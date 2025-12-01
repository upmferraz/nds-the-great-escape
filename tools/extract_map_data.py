import re
import os

INPUT_ASM = "../zxsrc/MEMDUMP.asm"
OUTPUT_H = "../include/MapData.h"
OUTPUT_CPP = "../source/MapData.cpp"

def parse_asm_data(filename):
    data = {}
    current_label = None
    
    print(f"A ler {filename}...")
    try:
        with open(filename, 'r', encoding='latin-1') as f:
            for line in f:
                # 1. Limpeza básica
                line = line.split(';')[0].strip()
                if not line: continue # Ignora linhas vazias
                
                # 2. Deteta Labels (Ex: "super_tiles db ..." OU "super_tiles:")
                # Regex procura uma palavra no inicio da linha que não seja 'db', 'dw', etc
                label_match = re.match(r'^([a-zA-Z_][a-zA-Z0-9_]*)(:|\s+db|\s+dw)?', line)
                
                is_data_line = line.startswith("db") or line.startswith("dw")
                
                if label_match and not is_data_line:
                    # Encontrámos uma nova label!
                    possible_label = label_match.group(1)
                    # Filtra palavras reservadas do ASM
                    if possible_label not in ['db', 'dw', 'dd', 'seg000', 'end', 'public', 'include']:
                        current_label = possible_label
                        data[current_label] = []
                        # print(f"Label encontrada: {current_label}")

                # 3. Extrai dados (db)
                # Se a linha tem 'db', adiciona ao current_label
                if "db " in line or line.startswith("db"):
                    if not current_label: continue
                    
                    # Remove o 'db' e separa
                    content = line.split("db")[-1].strip()
                    items = [x.strip() for x in content.split(',')]
                    
                    for item in items:
                        val = 0
                        try:
                            if item.endswith('h'):
                                val = int(item[:-1], 16)
                            elif item.isdigit():
                                val = int(item)
                            # Ignora '?' e outros lixos
                            data[current_label].append(val)
                        except:
                            pass

    except FileNotFoundError:
        print("ERRO: Ficheiro ASM não encontrado.")
        return {}
    
    return data

def write_cpp(data):
    print("A gerar C++...")
    with open(OUTPUT_CPP, 'w') as f:
        f.write('#include "MapData.h"\n\n')
        
        # Tiles Gráficos
        for i in range(4):
            key = f'exterior_tiles_{i}'
            if key in data:
                f.write(f'const u8 {key}[] = {{\n')
                for j, b in enumerate(data[key]):
                    f.write(f'0x{b:02X},')
                    if (j+1)%16==0: f.write('\n')
                f.write('};\n')
                f.write(f'const int {key}_size = sizeof({key});\n\n')
            else:
                f.write(f'const u8 {key}[] = {{0}};\nconst int {key}_size=1;\n')

        # Super Tiles (Raw) - Aqui está o segredo
        if 'super_tiles' in data:
            raw = data['super_tiles']
            print(f"Super Tiles extraídos: {len(raw)} bytes") 
            # Se este número for baixo (<5000), a extração falhou.
            # O mapa completo deve ter >10000 bytes.
            
            f.write('const u8 super_tiles_raw[] = {\n')
            for j, b in enumerate(raw):
                f.write(f'0x{b:02X},')
                if (j+1)%16==0: f.write('\n')
            f.write('};\n')
            f.write(f'const int super_tiles_raw_size = sizeof(super_tiles_raw);\n')
        else:
             print("AVISO: Label 'super_tiles' não encontrada no ASM!")

    with open(OUTPUT_H, 'w') as f:
        f.write('#pragma once\n#include <nds.h>\n\n')
        for i in range(4):
            f.write(f'extern const u8 exterior_tiles_{i}[];\nextern const int exterior_tiles_{i}_size;\n')
        f.write('extern const u8 super_tiles_raw[];\n')
        f.write('extern const int super_tiles_raw_size;\n')

d = parse_asm_data(INPUT_ASM)
write_cpp(d)
print("Concluído.")
