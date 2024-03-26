
# LilyGO T-Watch system and Rasberry Pi application

## Introduction

This Github repository consists of an embedded system (TTGo_FW folder) that is deployed on a programmable smart watch and a Rasberry Pi (RPi) application (RPi folder) that supports certain functions of the watch. The hosting platform for the embedded system is the LilyGO T-Watch-2020-V3 smart watch, more information can be found at https://www.lilygo.cc/products/t-watch-2020-v3. The scope of this project was to develop a few features on the smartwatch firmware, which is a VS Code project. These features are:

1. Step counting
2. Traveled distance calculation
3. Hike session data saving
4. Data transfer to the RPi via Bluetooth

All the features of the smart watch are implemented in the TTGo_FW/main.cpp file. The RPi application, which was programmed in Python, was deployed on the RPi 400 Kit. It implemented the following features:

1. Receiving hike sessions data via Bluetooth
2. Creating and updating the database on the RPi
3. Hosting a web server and displaying the hiking sessions in a web UI

These features are implemented on numerous files in the RPi folder. The web UI, which is created by the wserver.py file, is still in the development stage. This is due to the product still being in the prototyping phase.

## How to get started

To get started on the project, you should first have git on your machine. If not, then follow the instructions at https://git-scm.com/book/en/v2/Getting-Started-Installing-Git. To clone this repository, open git bash (or whatever command prompt that supports Git) and navigate into the folder you want to clone this repository into (e.g. Documents). Next, simply type **git clone link_to_this_repository** into the command prompt and hit enter. You can acquire the link by clicking on green "<> Code" button on the starting page of the remote repository and choosing a suitable link format. After successfully cloning the git project, you should see it in the file explorer.

The smart watch system can be developed on Windows, Linux or MacOS system. The recommendation is VSCode (v1.85) is recommended. With this, PlatformIO (Core v6.1) with ESP-IDF (ESP32 platform) is also needed. This can be simply installed in VSCode by navigating into the extensions sidebar, typing in "PlatformIO" and installing the Platform IO IDE. You can now open the files in the Platform IO extension in VSCode and start working on them.

For developing the RPi application, the following dependencies are needed: pybluez v.0.23, flask v.1.1.2 and pillow v. 8.1.2. These are somewhat old versions of the packages, so a virtual environment is recommended (e.g. Anaconda). To set up and a virtual environment with Anaconda, you can follow the instructions below:

1. Check if you have Anaconda on your machine by typing "Anaconda Prompt" after pressing the start button in the bottom-left corner of your screen. If nothing pops up, you should install Anaconda from https://docs.anaconda.com/free/anaconda/install/index.html.
2. Open Anaconda Prompt and navigate to ".../anaconda3/envs/" folder.
3. Run **conda create -n name_of_virtual_env** to create the virtual environment. Type "y" in the command if asked to do so.
4. Run **conda activate name_of_virtual_env** to activate the freshly created virtual environment.
5. Run **conda install python=3.11** to install an older version of python. Type "y" when prompted.
6. Navigate to the RPi application root folder (RPi) and run **pip install -r requirements.txt** to install the listed packages. If you don't encounter any issues, you can skip to step 7. If though, try installing pybluez v.0.23 from a certain Github repository by running **pip install git+https://github.com/pybluez/pybluez.git#egg=pybluez**. Then you should comment out the line that specifies the pybluez version in requirements.txt and run **pip install -r requirements.txt** again.
7. Open the RPi folder in VSCode and activate the virtual environment with the name that you chose from the button next to the bell in the bottom right corner. Please note that if you had VSCode open while installing the dependencies, you may ave to restart it for the changes to take effect.
8. You can now start working on the RPi application.

## Further development

Ideas for future development on the product include:

1. Communication between the smart watch and the RPi over WIFI
2. GPS tracking for more accurate travel distance and calorie calculation
3. More vivid and reactive graphical user interface (GUI) for the watch

Have fun developing the smart watch!
