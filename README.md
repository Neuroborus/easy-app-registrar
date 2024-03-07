# Easy App Registar
![](https://raw.githubusercontent.com/Neuroborus/goldenrandom/main/docs/doc.gif)

An easy way to register any app/script as a system application with a launcher </br>
QT/C++ </br>

### Tested and developed on Ubuntu 22

## UI legend

![](https://raw.githubusercontent.com/Neuroborus/goldenrandom/main/docs/ui.png)

`App name` - what you name the application </br>
`App filepath` - full path to the executable file </br>
`Icon filepath` - the full path to your icon </br>
`Global installation / User installation` - type of installation (for user or whole system) </br>
</br>
`Save resources` - an option that allows you to securely save the files you use in the application folder. Then, if the original specified files are lost, the installed application will remain functional. it is recommended to enable. It is also possible to save other necessary folders/files that the application depends on </br>
</br>
`Edit mode` - once enabled, allows you to manually make changes to the .desktop file that will be generated when the launcher is created </br>
</br>
`Start` - starts the application installation according to the specified settings </br>
</br>
`Config` - allows you to manually change the configuration, or reset it altogether </br>
</br>
`Load defaults / Save as default` - фllows you to save the field state as a template and unload when the application is used again </br>

## Uninstalling installed app
Files for applications are created in several places. All paths can be seen in the config: </br>
`appsDir` - application folder, where its config and saved files of installed applications are stored </br>
`globalApplicationsPath` - the launcher is installed there if global installation is selected </br>
`localApplicationsPath` - the launcher is installed there if user installation is selected </br>
</br>
You can see between `Launcher name` and `Folder name`, which will also help you find the files </br>
