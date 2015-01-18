var component;
var sprite;

function createObjects(name, appWindow , parent) {
    component = Qt.createComponent(name + ".qml");
    if (component.status == Component.Ready)
        finishCreation(appWindow, parent);
    else
        component.statusChanged.connect(finishCreation);
}

function finishCreation(appWindow, parent) {
    if (component.status == Component.Ready) {
        sprite = component.createObject(appWindow);
       // sprite.anchors.top = appWindow.anchors.bottom;
        sprite.y = 80;
        sprite.x = 20;
        if (sprite == null) {
            // Error Handling
            console.log("Error creating object");
        }
    } else if (component.status == Component.Error) {
        // Error Handling
        console.log("Error loading component:", component.errorString());
    }
}
