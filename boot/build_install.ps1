# Aurora OS - 构建安装镜像
$projectRoot = (Get-Location).Path

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Building Aurora OS Installer" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

# 1. 编译 install.c
Write-Host "[1/5] Compiling install.c..." -ForegroundColor Yellow
i686-elf-gcc -m32 -ffreestanding -fno-builtin -fno-stack-protector -Wall -Wextra -c "$projectRoot\install\install.c" -o "$projectRoot\install\install.o"
if ($LASTEXITCODE -ne 0) { exit 1 }

# 2. 链接
Write-Host "[2/5] Linking installer..." -ForegroundColor Yellow
i686-elf-ld -m elf_i386 -Ttext 0x20000 -o "$projectRoot\install\install.elf" "$projectRoot\install\install.o"
if ($LASTEXITCODE -ne 0) { exit 1 }

# 3. 提取二进制
Write-Host "[3/5] Extracting binary..." -ForegroundColor Yellow
i686-elf-objcopy -O binary "$projectRoot\install\install.elf" "$projectRoot\install\install.bin"
if ($LASTEXITCODE -ne 0) { exit 1 }

# 4. 编译 boot.asm 和 loader.asm
Write-Host "[4/5] Assembling boot loaders..." -ForegroundColor Yellow
nasm -f bin "$projectRoot\install\boot.asm" -o "$projectRoot\install\boot.bin"
nasm -f bin "$projectRoot\install\loader.asm" -o "$projectRoot\install\loader.bin"

# 5. 创建镜像
Write-Host "[5/5] Creating disk image..." -ForegroundColor Yellow

$imgSize = 1474560
$img = [byte[]]::new($imgSize)

$bootBytes = [System.IO.File]::ReadAllBytes("$projectRoot\install\boot.bin")
$loaderBytes = [System.IO.File]::ReadAllBytes("$projectRoot\install\loader.bin")
$installBytes = [System.IO.File]::ReadAllBytes("$projectRoot\install\install.bin")

# boot.bin -> 扇区 0 (偏移 0)
[System.Array]::Copy($bootBytes, 0, $img, 0, $bootBytes.Length)

# loader.bin -> 扇区 1-2 (偏移 512)
[System.Array]::Copy($loaderBytes, 0, $img, 512, $loaderBytes.Length)

# install.bin -> 扇区 3 开始 (偏移 1536)
[System.Array]::Copy($installBytes, 0, $img, 1536, $installBytes.Length)

[System.IO.File]::WriteAllBytes("$projectRoot\aurora_install.img", $img)

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  aurora_install.img created!" -ForegroundColor Green
Write-Host "  Size: $($img.Length) bytes" -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "To run installer:" -ForegroundColor Cyan
Write-Host "  qemu-system-x86_64 -m 64 -drive format=raw,file=aurora_install.img,if=floppy -drive format=raw,file=aurora_hdd.img,if=ide" -ForegroundColor White