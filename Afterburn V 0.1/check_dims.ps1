Add-Type -AssemblyName System.Drawing

$files = @(
    "Asset/Car.png",
    "Asset/Explosion.png",
    "Asset/Police_Animation/Police 1.png",
    "Asset/Police_Animation/Police 2.png",
    "Asset/Police_Animation/Police 3.png",
    "Asset/projectile.png",
    "Asset/Star System.png",
    "Asset/Special Enemy 1.png",
    "Asset/Special Effect.png",
    "Asset/Oil Puddle.png",
    "Asset/truck.png",
    "Asset/Power Deployment.png",
    "Asset/Power Orb.png",
    "Asset/torpedo.png",
    "Asset/Boss 1.png"
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
