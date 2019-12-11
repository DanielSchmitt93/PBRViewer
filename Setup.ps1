# Author: Daniel Schmitt
# ----------------------------
# Description:
# This command file extracts the zipped resource files for PBRViewer.
# ----------------------------
# Notes:
# Please execute this file before starting PBRViewer for the first time.
# If you get the error: "Running scripts is disabled on this system" please perform "set-executionpolicy remotesigned" in a powershell window with administrative rights.
# See https://www.faqforge.com/windows/windows-powershell-running-scripts-is-disabled-on-this-system/ for more information.
# ----------------------------

$rootDir = $PSScriptRoot
$resourcesPath = $rootDir + "/resources"

Write-Host "Extract 3D models to" $resourcesPath
Expand-Archive -LiteralPath  "$rootDir\resources_compressed\objects\objects.zip" -DestinationPath $resourcesPath -Force

Write-Host "Extract skyboxes to" $resourcesPath
Expand-Archive -LiteralPath  "$rootDir\resources_compressed\skybox\skybox.zip" -DestinationPath $resourcesPath -Force

Write-Host "Setup finished"