Add-Type -AssemblyName System.Drawing

$files = @(
    "Asset/Car.png",
    "Asset/Explosion.png",
    "Asset/Police_Animation/Police 1.png",
    "Asset/Police_Animation/Police 2.png",
    "Asset/Police_Animation/Police 3.png",
    "Asset/projectile.png",
    "Asset/Star System.png"
)

foreach ($f in $files) {
    if (Test-Path $f) {
        $fullPath = Resolve-Path $f
        $img = [System.Drawing.Image]::FromFile($fullPath)
        Write-Output "$f : $($img.Width) x $($img.Height)"
        $img.Dispose()
    } else {
        Write-Output "$f : Not Found"
    }
}
