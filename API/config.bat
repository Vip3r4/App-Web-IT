@echo off
title Gestionnaire Serre Connectee
color 0B

echo ======================================================
echo    INSTALLATION DES DEPENDANCES ET LANCEMENT
echo ======================================================

:: 1. Installation des bibliotheques si besoin
echo [1/3] Verification des bibliotheques Python...
python -m pip install fastapi uvicorn pyserial requests --trusted-host pypi.org --trusted-host files.pythonhosted.org

echo.
echo [2/3] Lancement de l'API (Fenetre separee)...
:: On lance l'API dans une nouvelle fenetre pour qu'elle tourne en arriere-plan
start "API Serre" cmd /k "python -m uvicorn main:app --reload"

timeout /t 5

echo.
echo [3/3] Lancement du Pont Arduino...
:: On lance le script de lecture serie dans la fenetre actuelle
python bridge.py
python main.py

pause
