# Define the thread counts you want to test
$threadCounts = 16 # Adjust based on your CPU cores/threads

# Path to your compiled executable
$executablePath = ".\main.exe" # Assumes it's in the same directory as the script

Write-Host "Starting Raytracer Profiling..."
Write-Host "================================="

foreach ($threads in $threadCounts) {
    Write-Host "Testing with $threads threads..."

    # Set the OMP_NUM_THREADS environment variable for the command
    $env:OMP_NUM_THREADS = $threads

    # Measure the time taken to run the executable
    # Redirect standard output (PPM image) to a file
    # Keep standard error (clog progress) visible in the console
    $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
    & $executablePath > "output_${threads}_threads.ppm"
    $stopwatch.Stop()

    $elapsedSeconds = $stopwatch.Elapsed.TotalSeconds
    Write-Host "Time taken ($threads threads): $elapsedSeconds seconds"
    Write-Host "---------------------------------"
}

Write-Host "Profiling finished."

# Optional: Clean up the environment variable in the current session
# Remove-Item Env:\OMP_NUM_THREADS