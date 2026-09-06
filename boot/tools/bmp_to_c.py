# tools/bmp_to_c.py
from PIL import Image
import os

# 直接写绝对路径 (根据你的项目路径)
project = r'C:\Users\Administrator\Documents\Aurora OS'

image_path = os.path.join(project, 'resources', 'wallpaper.bmp')
output_path = os.path.join(project, 'kernel', 'src', 'gui', 'wallpaper_data.c')

print(f'📁 壁纸文件: {image_path}')

try:
    img = Image.open(image_path)
    img.thumbnail((640, 480), Image.Resampling.LANCZOS)
    img = img.convert('RGB')
    
    width, height = img.size
    pixels = list(img.getdata())
    
    # RLE压缩
    compressed = []
    i = 0
    total = len(pixels)
    
    while i < total:
        count = 1
        r, g, b = pixels[i]
        while i + count < total and count < 255:
            r2, g2, b2 = pixels[i + count]
            if r2 != r or g2 != g or b2 != b:
                break
            count += 1
        compressed.append(count)
        compressed.append(r)
        compressed.append(g)
        compressed.append(b)
        i += count
    
    with open(output_path, 'w') as f:
        f.write('// 压缩壁纸数据\n')
        f.write('#include <stddef.h>\n\n')
        f.write('const unsigned char wallpaper_compressed[] = {\n')
        
        for i in range(0, len(compressed), 12):
            chunk = compressed[i:i+12]
            line = '    ' + ', '.join(f'0x{x:02X}' for x in chunk)
            if i + 12 < len(compressed):
                line += ','
            f.write(line + '\n')
        
        f.write('};\n\n')
        f.write(f'const size_t wallpaper_compressed_size = {len(compressed)};\n')
        f.write(f'const int wallpaper_width = {width};\n')
        f.write(f'const int wallpaper_height = {height};\n')
    
    print(f'✅ 壁纸转换成功！')
    print(f'📐 尺寸: {width}x{height}')
    print(f'📦 压缩后: {len(compressed)} 字节')
    print(f'💾 输出: {output_path}')

except FileNotFoundError:
    print(f'❌ 找不到壁纸文件啦 (´;︵;`)')
    print(f'💡 请确认: {image_path} 存在')
    print(f'💡 文件名是 wallpaper.bmp 哦！')