@echo off
title Gestionnaire SmartGH
color 0B

echo ======================================================
echo    INSTALLATION DES DEPENDANCES ET LANCEMENT
echo ======================================================

echo [1/3] Verification des bibliotheques Python...
python -m pip install fastapi uvicorn pyserial requests --trusted-host pypi.org --trusted-host files.pythonhosted.org

echo.
echo [2/3] Lancement de l'API (Fenetre separée)...
start "API Serre" cmd /k "python -m uvicorn main:app --reload --host 0.0.0.0 --port 8000"

timeout /t 5

echo.
echo [3/3] Lancement du Pont Arduino...
python bridge.py
python main.py

echo ======================================================
echo    PROGRAMME PRET A L'UTILISATION
echo ======================================================

pause
