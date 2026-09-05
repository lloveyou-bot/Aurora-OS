# ============================================
# Aurora OS - 完整构建脚本 v0.4
# ============================================

$projectRoot = (Get-Location).Path

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Aurora OS Build Script v0.4" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Project root: $projectRoot" -ForegroundColor Gray
Write-Host ""

# ============================================
# 1. 编译所有 C 文件
# ============================================
Write-Host "[1/9] Compiling C files..." -ForegroundColor Yellow

$cFiles = @(
    "main.c",
    "terminal.c",
    "idt.c",
    "pic.c",
    "keyboard.c",
    "mouse.c",
    "shell.c",
    "commands.c",
    "fs.c",
    "explorer.c",
    "graphics/vbe.c",
    "graphics/graphics.c",
    "gui/window.c",
    "gui/desktop.c",
    "font/hzk16.c"
)

foreach ($file in $cFiles) {
    $name = $file -replace "/", "\"
    Write-Host "  - $name" -ForegroundColor Gray
    $output = $file -replace "/", "_" -replace "\.c$", ".o"
    i686-elf-gcc -m32 -ffreestanding -fno-builtin -fno-stack-protector -Wall -Wextra -c "$projectRoot\kernel\src\$file" -o "$projectRoot\kernel\$output"
    if ($LASTEXITCODE -ne 0) { 
        Write-Host "ERROR: $file compilation failed!" -ForegroundColor Red
        exit 1 
    }
}

Write-Host ""

# ============================================
# 2. 汇编 interrupt.asm
# ============================================
Write-Host "[2/9] Assembling interrupt.asm..." -ForegroundColor Yellow
nasm -f elf32 "$projectRoot\boot\interrupt.asm" -o "$projectRoot\kernel\interrupt.o"
if ($LASTEXITCODE -ne 0) { 
    Write-Host "ERROR: interrupt.asm compilation failed!" -ForegroundColor Red
    exit 1 
}
Write-Host "  OK" -ForegroundColor Green
Write-Host ""

# ============================================
# 3. 嵌入资源文件
# ============================================
Write-Host "[3/9] Embedding resources..." -ForegroundColor Yellow

Push-Location "$projectRoot\kernel"

# HZK16 中文字库
if (Test-Path "$projectRoot\resources\hzk16.bin") {
    Write-Host "  - Embedding hzk16.bin..." -ForegroundColor Gray
    i686-elf-ld -r -b binary -o hzk16_data.o "$projectRoot\resources\hzk16.bin" 2>$null
    if ($LASTEXITCODE -eq 0) { 
        Write-Host "    hzk16_data.o created" -ForegroundColor Green
    } else {
        Write-Host "    WARNING: hzk16.bin embedding failed!" -ForegroundColor Yellow
    }
} else {
    Write-Host "  - WARNING: hzk16.bin not found" -ForegroundColor Yellow
}

# 壁纸图片（如果存在且不太大）
if (Test-Path "$projectRoot\resources\wallpaper.bmp") {
    $wallpaperSize = (Get-Item "$projectRoot\resources\wallpaper.bmp").Length
    if ($wallpaperSize -lt 50000) {
        Write-Host "  - Embedding wallpaper.bmp..." -ForegroundColor Gray
        i686-elf-ld -r -b binary -o wallpaper.o "$projectRoot\resources\wallpaper.bmp" 2>$null
        if ($LASTEXITCODE -eq 0) { 
            Write-Host "    wallpaper.o created" -ForegroundColor Green
        } else {
            Write-Host "    WARNING: wallpaper.bmp embedding failed!" -ForegroundColor Yellow
        }
    } else {
        Write-Host "  - WARNING: wallpaper.bmp too large ($wallpaperSize bytes), skipping" -ForegroundColor Yellow
    }
} else {
    Write-Host "  - WARNING: wallpaper.bmp not found, using gradient" -ForegroundColor Yellow
}

Pop-Location
Write-Host ""

# ============================================
# 4. 链接所有 .o 文件
# ============================================
Write-Host "[4/9] Linking..." -ForegroundColor Yellow

Push-Location "$projectRoot\kernel"

$linkFiles = @(
    "main.o",
    "terminal.o",
    "idt.o",
    "pic.o",
    "keyboard.o",
    "mouse.o",
    "shell.o",
    "commands.o",
    "fs.o",
    "explorer.o",
    "vbe.o",
    "graphics.o",
    "window.o",
    "desktop.o",
    "hzk16.o",
    "interrupt.o"
)

if (Test-Path "hzk16_data.o") {
    $linkFiles += "hzk16_data.o"
}
if (Test-Path "wallpaper.o") {
    $linkFiles += "wallpaper.o"
}

$linkCmd = "i686-elf-ld -m elf_i386 -T `"$projectRoot\kernel\linker.ld`" -o `"$projectRoot\kernel\kernel.elf`" " + ($linkFiles -join " ")
Write-Host "  Linking $($linkFiles.Count) files..." -ForegroundColor Gray
Invoke-Expression $linkCmd

Pop-Location

if ($LASTEXITCODE -ne 0) { 
    Write-Host "ERROR: Linking failed!" -ForegroundColor Red
    exit 1 
}
Write-Host "  OK" -ForegroundColor Green
Write-Host ""

# ============================================
# 5. 提取二进制
# ============================================
Write-Host "[5/9] Extracting binary..." -ForegroundColor Yellow
i686-elf-objcopy -O binary "$projectRoot\kernel\kernel.elf" "$projectRoot\kernel\kernel.bin"
if ($LASTEXITCODE -ne 0) { 
    Write-Host "ERROR: objcopy failed!" -ForegroundColor Red
    exit 1 
}
Write-Host "  OK" -ForegroundColor Green
Write-Host ""

# ============================================
# 6. 编译 boot.asm
# ============================================
Write-Host "[6/9] Compiling boot.asm..." -ForegroundColor Yellow
nasm -f bin "$projectRoot\boot\boot.asm" -o "$projectRoot\boot\boot.bin"
if ($LASTEXITCODE -ne 0) { 
    Write-Host "ERROR: boot.asm compilation failed!" -ForegroundColor Red
    exit 1 
}
Write-Host "  OK" -ForegroundColor Green
Write-Host ""

# ============================================
# 7. 编译 loader.asm
# ============================================
Write-Host "[7/9] Compiling loader.asm..." -ForegroundColor Yellow
nasm -f bin "$projectRoot\boot\loader.asm" -o "$projectRoot\boot\loader.bin"
if ($LASTEXITCODE -ne 0) { 
    Write-Host "ERROR: loader.asm compilation failed!" -ForegroundColor Red
    exit 1 
}
Write-Host "  OK" -ForegroundColor Green
Write-Host ""

# ============================================
# 8. 创建镜像
# ============================================
Write-Host "[8/9] Creating disk image..." -ForegroundColor Yellow

$imgSize = 1474560
$img = [byte[]]::new($imgSize)

$bootBytes = [System.IO.File]::ReadAllBytes("$projectRoot\boot\boot.bin")
$loaderBytes = [System.IO.File]::ReadAllBytes("$projectRoot\boot\loader.bin")
$kernelBytes = [System.IO.File]::ReadAllBytes("$projectRoot\kernel\kernel.bin")

Write-Host "  boot.bin: $($bootBytes.Length) bytes" -ForegroundColor Gray
Write-Host "  loader.bin: $($loaderBytes.Length) bytes" -ForegroundColor Gray
Write-Host "  kernel.bin: $($kernelBytes.Length) bytes" -ForegroundColor Gray

[System.Array]::Copy($bootBytes, 0, $img, 0, $bootBytes.Length)
[System.Array]::Copy($loaderBytes, 0, $img, 512, $loaderBytes.Length)

if ($kernelBytes.Length -le 63488) {
    [System.Array]::Copy($kernelBytes, 0, $img, 1024, $kernelBytes.Length)
} else {
    Write-Host "  WARNING: Kernel too large! Truncating to 62 sectors..." -ForegroundColor Yellow
    [System.Array]::Copy($kernelBytes, 0, $img, 1024, 63488)
}

[System.IO.File]::WriteAllBytes("$projectRoot\aurora.img", $img)

Write-Host "  OK" -ForegroundColor Green
Write-Host ""

# ============================================
# 9. 显示结果
# ============================================
Write-Host "[9/9] Build Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  aurora.img created successfully!" -ForegroundColor Green
Write-Host "  Kernel size: $($kernelBytes.Length) bytes" -ForegroundColor Yellow
Write-Host "  Image size: $($img.Length) bytes" -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

Write-Host "To run Aurora OS (Text Mode):" -ForegroundColor Cyan
Write-Host "  qemu-system-x86_64 -m 32 -drive file=aurora.img,format=raw,if=floppy -boot a -no-reboot -no-shutdown" -ForegroundColor White
Write-Host ""
Write-Host "To run Aurora OS (Graphics Mode):" -ForegroundColor Cyan
Write-Host "  qemu-system-x86_64 -m 64 -vga std -drive file=aurora.img,format=raw,if=floppy -boot a -no-reboot -no-shutdown" -ForegroundColor White