import os

try:
    print("Downloading FRL now. Internet connection is required. Nothing will install if there is no internet.")
    if not os.path.exists("src/main/include"):
        print("Error! Could not find path 'src/main/include'.")
    else:
        try:
            if not os.path.exists("src/main/include/frl/base"):
                os.system("mkdir src/main/include/frl > /dev/null 2>&1")
                os.system("git clone https://github.com/Firestorm-Robotics-Code/FRL-New src/main/include/frl > /dev/null 2>&1")
        except:
            print("Error adding a new directory! Make sure that the path 'src/main/include' exists. Installing FRL will not work unless it does.")
    
        if os.path.exists("src/main/include/frl"):
            try:
                os.system("git add src/main/include/frl")
                #os.system("git clone https://github.com/Firestorm-Robotics-Code/FRL-New src/main/include/frl > /dev/null 2>&1")
                os.system("git pull origin main")
            except:
                print("Unable to install FRL! Internet connection required!")

except:
    print("Error while installing!")