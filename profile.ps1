# Define the thread counts you want to test
$threadCounts = 12, 10, 8, 6, 4, 2, 1 # Adjust based on your CPU cores/threads

# Path to your compiled executable
$executablePath = ".\sweepdynamic.exe" # Assumes it's in the same directory as the script

Write-Host "Starting Raytracer Profiling..."
Write-Host "================================="

foreach ($threads in $threadCounts) {
    Write-Host "Testing with $threads threads..."


    $env:OMP_NUM_THREADS = $threads

    & $executablePath > "output_${threads}_threads.ppm"

    Write-Host "---------------------------------"
}

Write-Host "Profiling finished."

# Optional: Clean up the environment variable in the current session
# Remove-Item Env:\OMP_NUM_THREADS