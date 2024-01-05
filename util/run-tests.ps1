$tmpfile = New-TemporaryFile | Select -ExpandProperty FullName
$tmpfile_lin = $tmpfile -replace 'C:', '/c' -replace '\\', '/'

$lindir=$PSScriptRoot -replace '\\', '/' -replace 'C:', '/c'
$proc = Start-Process -NoNewWindow -PassThru -FilePath "$env:Programfiles\Git\git-bash.exe" `
	-ArgumentList "-c", "'$lindir/run-tests.sh > $tmpfile_lin 2>&1'"

$proc.WaitForExit()

echo "Test output can be found at $tmpfile"
echo "--- TEST LOG OUTPUT ---"
cat $tmpfile
