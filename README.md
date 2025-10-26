🧩 TwinWire — Press the Core

TwinWire es un juego de acción arcade con estética retro inspirado en los clásicos de los 90, desarrollado completamente en C++17 y SFML 3.0.0.
El proyecto fue diseñado como un ejercicio técnico y artístico, enfocado en arquitectura modular, animaciones frame a frame, y sistemas personalizados para UI, recursos y lógica de juego.

🎮 Gameplay

Controlás a un pequeño aventurero conectado por filamentos de energía.
Tu objetivo es sobrevivir y derrotar al boss, evitando patrones de proyectiles cada vez más complejos y utilizando los filamentos A/B para atacar o romper estructuras.

Cada enfrentamiento propone una danza de precisión y reflejos, donde cada movimiento cuenta.

🕹️ Controles
Acción	Tecla / Botón
Moverse	A / D
Filamento A	Click Izquierdo
Filamento B	Click Derecho
Filamento A + B	Romper pilar
Pausa	ESC

💡 Algunos patrones cambian de ángulo aleatoriamente.
El menú principal incluye opciones de sonido, volumen y créditos.

⚙️ Características técnicas

Lenguaje: C++17

Framework: SFML 3.0.0

Arquitectura modular con principios SOLID + KISS

Sistema de animación personalizado (frame-by-frame)

FSM ligera para el comportamiento del boss

Object Pool para gestión eficiente de proyectiles

ResourceManager y UIManager propios

Escenas desacopladas (Main Menu, Gameplay, Win Scene, Pause Layer)

Audio Manager y configuración persistente entre escenas

Preparado para distribución en Release x64

🔊 Recursos utilizados

Arte del jefe y jugador:
🎨 Macarena Vera

Fondos y assets:

Free Sky Backgrounds — Free Game Assets

Free Ancient Temple Pixel Game Backgrounds

Música / SFX:

Touhou Bunkachou (Shoot the Bullet)

Touhou Koumakyou (The Embodiment of Scarlet Devil)

HRtP Shingyoku’s Theme – The Positive and Negative

Magical Astronomy – Greenwich in the Sky

Touhou Phantasmagoria of Dim Dream – Victory Demonstration

💾 Compilación y ejecución
Requisitos

Compilador: MSVC (Visual Studio 2022 o superior)

C++17

SFML 3.0.0 (no 3.0.1)

Windows x64

Instrucciones

Cloná el repositorio:

git clone https://github.com/Shauzen-Prog/TwinWire.git


Abrí el proyecto en Visual Studio.

Seleccioná la configuración Release | x64.

Compilá (Build > Rebuild Solution).

Verificá que los recursos estén junto al .exe:

bin\Release\
├─ TwinWire_SFML.exe
├─ res\
└─ *.dll


Ejecutá TwinWire_SFML.exe.

🔗 Enlaces

🎮 Jugar / Descargar (Itch.io):
👉 https://alesc.itch.io/twinwire-press-the-core

💻 Repositorio oficial:
👉 https://github.com/Shauzen-Prog/TwinWire

👥 Créditos

Dirección técnica y programación principal
🎮 Alexis Escobar

Arte del jefe y jugador
🎨 Macarena Vera

Fondos y assets
🏞️ Free Sky Backgrounds, Free Ancient Temple Pixel Game Backgrounds

Música / SFX
🎵 Touhou Project (ZUN) – uso no comercial / educativo

📜 Licencia

Este proyecto se publica con fines educativos y de demostración técnica.
No está destinado a distribución comercial.
© 2025 AL Esc Games
