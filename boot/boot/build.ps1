# ============================================
# Aurora OS - 构建脚本 v0.5 (with Network)
# ============================================

$projectRoot = (Get-Location).Path

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Aurora OS Build Script v0.5" -ForegroundColor Cyan
Write-Host "  with Network Support" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Project root: $projectRoot" -ForegroundColor Gray
Write-Host ""

# 1. 编译所有 C 文件
Write-Host "[1/5] Compiling C files..." -ForegroundColor Yellow

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
    "pci\pci.c",
    "net\ne2000.c",
    "net_test.c"
)

foreach ($file in $cFiles) {
    $displayName = $file -replace "/", "\"
    Write-Host "  - $displayName" -ForegroundColor Gray
    $basename = Split-Path $file -Leaf
    $output = $basename -replace "\.c$", ".o"
    # 🆕 添加 -I 参数，让编译器在 kernel/src 下找头文件
    i686-elf-gcc -m32 -ffreestanding -fno-builtin -fno-stack-protector -Wall -Wextra -I"$projectRoot\kernel\src" -c "$projectRoot\kernel\src\$file" -o "$projectRoot\kernel\$output"
    if ($LASTEXITCODE -ne 0) { 
        Write-Host "ERROR: $file compilation failed!" -ForegroundColor Red
        exit 1 
    }
}

Write-Host ""

# 2. 汇编 interrupt.asm
Write-Host "[2/5] Assembling interrupt.asm..." -ForegroundColor Yellow
nasm -f elf32 "$projectRoot\boot\interrupt.asm" -o "$projectRoot\kernel\interrupt.o"
if ($LASTEXITCODE -ne 0) { 
    Write-Host "ERROR: interrupt.asm compilation failed!" -ForegroundColor Red
    exit 1 
}
Write-Host "  OK" -ForegroundColor Green
Write-Host ""

# 3. 链接
Write-Host "[3/5] Linking..." -ForegroundColor Yellow

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
    "pci.o",
    "ne2000.o",
    "net_test.o",
    "interrupt.o"
)

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

# 4. 提取二进制
Write-Host "[4/5] Extracting binary..." -ForegroundColor Yellow
i686-elf-objcopy -O binary "$projectRoot\kernel\kernel.elf" "$projectRoot\kernel\kernel.bin"
if ($LASTEXITCODE -ne 0) { 
    Write-Host "ERROR: objcopy failed!" -ForegroundColor Red
    exit 1 
}
Write-Host "  OK" -ForegroundColor Green
Write-Host ""

# 5. 创建镜像
Write-Host "[5/5] Creating disk image..." -ForegroundColor Yellow

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
    Write-Host "  WARNING: Kernel too large! Truncating..." -ForegroundColor Yellow
    [System.Array]::Copy($kernelBytes, 0, $img, 1024, 63488)
}

[System.IO.File]::WriteAllBytes("$projectRoot\aurora.img", $img)

Write-Host "  OK" -ForegroundColor Green
Write-Host ""

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  🌟 aurora.img created!" -ForegroundColor Green
Write-Host "  📦 Kernel size: $($kernelBytes.Length) bytes" -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

Write-Host "📖 Run with network:" -ForegroundColor White
Write-Host "  qemu-system-x86_64 -m 64 -vga std -drive file=aurora.img,format=raw,if=floppy -boot a -netdev user,id=net0 -device ne2k_pci,netdev=net0 -no-reboot -no-shutdown" -ForegroundColor Gray
Write-Host ""