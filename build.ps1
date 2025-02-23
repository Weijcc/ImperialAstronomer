$cpu = Get-WmiObject –class Win32_processor
$perfix = "C:\Application\Qt\6.8.2\msvc2022_64"
$location = (Get-Location).Path
$processors = $cpu.NumberOfLogicalProcessors
$directory = ""

$run = $null
$config = $null

foreach ($arg in $args) {
  if (($arg -eq "Debug") -or ($arg -eq "debug")) {
    $config = "Debug"
  }
  elseif (($arg -eq "Release") -or ($arg -eq "release")) {
    $config = "Release"
  }
  elseif (($arg -eq "Run") -or ($arg -eq "run")) {
    $run = "Run"
  }
}

if ($null -eq $config) {
  $config = "Debug"
}

Write-Host "cmake -DCMAKE_BUILD_TYPE=$config -DCMAKE_PREFIX_PATH=$perfix -G Ninja -B Build/$config" -ForegroundColor Green
cmake -DCMAKE_BUILD_TYPE="$config" -DCMAKE_PREFIX_PATH="$perfix" -G Ninja -B Build/$config

if ($?) {
  Write-Host "cmake --build Build/$config --target ImperialAstronomer -j $processors" -ForegroundColor Green
  $directory = "$location\Build\$config"
  cmake --build Build/$config --target ImperialAstronomer -j $processors

  if ($?) {
    if (($env:Path).indexOf("$perfix\bin") -eq -1) {
      $env:Path += ";$perfix\bin"
    }

    Write-Host "build success!" -ForegroundColor Green
    Write-Host "windeployqt $directory\ImperialAstronomer.exe" -ForegroundColor Green
    windeployqt "$directory\ImperialAstronomer.exe" | Out-Null

    if ($?) {
      if ($run -eq "Run") {
        Write-Host "execute..." -ForegroundColor Green
        $code = (Start-Process -NoNewWindow -Wait -FilePath "$directory\ImperialAstronomer.exe" -ArgumentList "-platform windows:fontengine=freetype" -PassThru).ExitCode
        
        if ($code -eq 0)
        {
          Write-Host "exit code $code" -ForegroundColor Green
        }
        else
        {
          Write-Host "exit code $code" -ForegroundColor Red
        }
      }
    }
    else {
      Write-Host "build fail!" -ForegroundColor Red
    }
  }
}
