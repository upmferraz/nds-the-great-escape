import re

INPUT_ASM = "../zxsrc/MEMDUMP.asm"

def parse_super_tiles(filename):
    data = []
    try:
        with open(filename, 'r', encoding='latin-1') as f:
            in_super_tiles = False
            for line in f:
                line = line.strip()
                if line.startswith("super_tiles"):
                    in_super_tiles = True
                
                # Se encontrarmos outra label grande, paramos (opcional, mas seguro)
                if in_super_tiles and ":" in line and not line.startswith("super_tiles"):
                    # break # Descomentar se houver labels no fim
                    pass

                if in_super_tiles and line.startswith("db"):
                    # Limpar comentários
                    line = line.split(';')[0]
                    parts = line.replace("db", "").split(",")
                    for p in parts:
                        p = p.strip()
                        if not p: continue
                        val = 0
                        if p.endswith('h'):
                            try: val = int(p[:-1], 16)
                            except: pass
                        elif p.isdigit():
                            val = int(p)
                        data.append(val)
    except Exception as e:
        print(f"Erro: {e}")
    return data

def print_map(data, offset, width, height=60):
    print(f"\n--- MAPA (Offset: {offset}, Width: {width}) ---")
    
    chars = " .:-=+*#%@" # Caracteres para representar densidade
    
    for y in range(height):
        row_str = ""
        for x in range(width):
            idx = offset + (y * width) + x
            if idx >= len(data): break
            
            val = data[idx]
            
            # Lógica de visualização:
            # 255 (0xFF) costuma ser vazio/transparente
            if val == 255:
                row_str += " " 
            # Valores pequenos costumam ser chão/paredes comuns
            elif val < 10:
                row_str += "."
            # Valores intermédios são objectos
            else:
                row_str += "#"
                
        print(f"{y:02d}: {row_str}")

# EXECUÇÃO
raw_data = parse_super_tiles(INPUT_ASM)
print(f"Total dados lidos: {len(raw_data)} bytes")

# O valor oficial do Assembly
OFFICIAL_OFFSET = 3450
OFFICIAL_WIDTH = 47

print_map(raw_data, OFFICIAL_OFFSET, OFFICIAL_WIDTH)
