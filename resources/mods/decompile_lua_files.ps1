# Expects OpenJDK to be extracted in the 'lua_tools' subdirectory
# (https://download.java.net/java/GA/jdk17.0.2/dfd4a8d0985749f896bed50d7138ee7f/8/GPL/openjdk-17.0.2_windows-x64_bin.zip)
$javaBinPath = [IO.Path]::Combine($pwd.Path, 'lua_tools', 'openjdk-17.0.2_windows-x64_bin\jdk-17.0.2\bin\java')

# Expects unluac.jar to be in the 'lua_tools' subdirectory
# (https://sourceforge.net/projects/unluac/)
$unluacPath = [IO.Path]::Combine($pwd.Path, 'lua_tools', 'unluac_2022_01_12.jar')

# Recurse through all subdirectories
$allFiles = Get-ChildItem -Recurse -Filter '*.lua.bin'

Write-Host 'Java path:' $javaBinPath
Write-Host 'Unluac path:' $unluacPath
Write-Host 'Decompiling' $allFiles.Length 'Lua files...'

$allFiles | ForEach-Object -Parallel {
    $bytecodePath = $_
    $sourceCodePath = ($_ -Replace '.lua.bin', '.lua')

    # & $javaBinPath '-jar' $unluacPath $luaBytecodePath > $luaSourceDest
    (& $using:javaBinPath '-jar' $using:unluacPath $bytecodePath) | Out-File -Path $sourceCodePath -Encoding ASCII
}

Write-Host 'Done.'