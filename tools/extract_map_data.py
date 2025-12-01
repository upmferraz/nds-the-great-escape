import re

INPUT_ASM = "../zxsrc/MEMDUMP.asm"
OUTPUT_H = "../include/MapData.h"
OUTPUT_CPP = "../source/MapData.cpp"

def parse_asm_data(filename):
    data = {}
    current_label = None
    
    try:
        with open(filename, 'r', encoding='latin-1') as f:
            for line in f:
                line = line.split(';')[0].strip()
                if not line: continue
                
                # Deteta labels
                label_match = re.match(r'^(\w+)\s+db\s+(.*)', line)
                if label_match:
                    current_label = label_match.group(1)
                    data[current_label] = []
                    values = label_match.group(2)
                elif current_label and line.startswith('db '):
                    values = line[3:]
                else:
                    if not line.startswith('db'): current_label = None
                    continue
                
                # Parse bytes
                if current_label:
                    items = [x.strip() for x in values.split(',')]
                    for item in items:
                        val = 0
                        if item.endswith('h'):
                            try: val = int(item[:-1], 16)
                            except: pass
                        elif item.isdigit(): val = int(item)
                        data[current_label].append(val)
    except FileNotFoundError:
        return {}
    return data

def write_cpp(data):
    with open(OUTPUT_CPP, 'w') as f:
        f.write('#include "MapData.h"\n\n')
        
        # Tiles Gráficos (Exterior)
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

        # DADOS BRUTOS DO MAPA (Super Tiles + Layout)
        if 'super_tiles' in data:
            f.write('const u8 super_tiles_raw[] = {\n')
            for j, b in enumerate(data['super_tiles']):
                f.write(f'0x{b:02X},')
                if (j+1)%16==0: f.write('\n')
            f.write('};\n')
            f.write(f'const int super_tiles_raw_size = sizeof(super_tiles_raw);\n')

    with open(OUTPUT_H, 'w') as f:
        f.write('#pragma once\n#include <nds.h>\n\n')
        for i in range(4):
            f.write(f'extern const u8 exterior_tiles_{i}[];\nextern const int exterior_tiles_{i}_size;\n')
        f.write('extern const u8 super_tiles_raw[];\n')
        f.write('extern const int super_tiles_raw_size;\n')

d = parse_asm_data(INPUT_ASM)
write_cpp(d)
print("MapData gerado com sucesso (RAW mode).")
