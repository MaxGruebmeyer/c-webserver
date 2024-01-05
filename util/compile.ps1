$tmpfile = New-TemporaryFile | Select -ExpandProperty FullName
$tmpfile_lin = $tmpfile -replace 'C:', '/c' -replace '\\', '/'

$proc = Start-Process -NoNewWindow -PassThru -FilePath "$env:Programfiles\Git\git-bash.exe" `
	-ArgumentList "-c", "'task build-shell > $tmpfile_lin 2>&1'"

$proc.WaitForExit()

echo "Build output can be found at $tmpfile"
echo "--- BUILD LOG OUTPUT ---"
cat $tmpfile
