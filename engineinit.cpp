#include "engineinit.h"
#include <QRegularExpression>

namespace gea
{

 //defining comon components
gea::Mesh EngineInit::sharedSphereMesh;
gea::Texture EngineInit::sharedSphereTexture;




// Defining static variables

QElapsedTimer gea::EngineInit::timer{};
float gea::EngineInit::lastTime{0.0f};
float gea::EngineInit::deltaTime{0.0f};
float gea::EngineInit::currentTime{0.0f};

//Fluid simulation
float gea::EngineInit::spawnTimer{0.0f};
int   gea::EngineInit::totalBallsToSpawn{0};
int   gea::EngineInit::ballsSpawned{0};

std::filesystem::path EngineInit::rootPath;
std::filesystem::path EngineInit::mEngineContentDirectoryPath;
std::filesystem::path EngineInit::mEngineAssetsDirectoryPath;
std::filesystem::path EngineInit::splashArtPath;


QDir EngineInit::mEngineContentDirectory;
QDir EngineInit::mEngineAssetsDirectory;

std::unique_ptr<RenderSystem>   EngineInit::RenderSystem{nullptr};
std::unique_ptr<CameraSystem>   EngineInit::CameraSystem{nullptr};
std::unique_ptr<InputSystem>    EngineInit::InputSystem{nullptr};
std::unique_ptr<PhysicsSystem>  EngineInit::PhysicsSystem{nullptr};
std::unique_ptr<fluidSystem>    EngineInit::fluidSystem{nullptr};
///
/// @brief The first thing that runs when starting the engine
///
void EngineInit::PreInitializeEngine()
{
    rootPath = std::filesystem::current_path();
    mEngineAssetsDirectoryPath = rootPath / "Engine/Assets";
    mEngineAssetsDirectory = QDir(mEngineAssetsDirectoryPath);

    if (!mEngineAssetsDirectory.exists()) mEngineAssetsDirectory.mkpath(".");

    QRegularExpression regex("^splash_art\\.[^\\.]+$");

    for (const QString &file : mEngineAssetsDirectory.entryList(QDir::Files))
    {
        if (regex.match(file).hasMatch())
        {
            splashArtPath = mEngineAssetsDirectory.absoluteFilePath(file).toStdString();
            break;
        }
    }
}

///
/// @brief Runs after the splash art, but before creating the MainWindow
///
void EngineInit::InitializeEngine()
{
    mEngineContentDirectoryPath = rootPath / "Content";
    mEngineContentDirectory = QDir(mEngineContentDirectoryPath);

    if (!mEngineContentDirectory.exists()) mEngineContentDirectory.mkpath(".");

}

void EngineInit::PostInitalizeEngineInitalization(Renderer* renderSurface)
{
    sharedSphereTexture.path = "../../Assets/Textures/texture.jpg";
    sharedSphereMesh.path = "../../Assets/Models/Sphere.obj";

    if (!timer.isValid())
        timer.start();


    // initialize systems
    RenderSystem   = std::make_unique<gea::RenderSystem>();
    CameraSystem   = std::make_unique<gea::CameraSystem>();
    InputSystem    = std::make_unique<gea::InputSystem>();
    PhysicsSystem  = std::make_unique<gea::PhysicsSystem>();
    fluidSystem    = std::make_unique<gea::fluidSystem>();
    // create camera entities
    gea::Entity CameraMan = entityManager.createEntity();

    //Create camera components
    gea::Camera cam1;
    cam1.isActive = true;

    //Connect components to entities CAMERA
    registry.addComponent(CameraMan.mEntityID,cam1);

    QLog(mat4ToQString(cam1.mViewMatrix),"blue");
    QLog(mat4ToQString(cam1.mProjectionMatrix),"blue");
    QLog(&" " [ cam1.isActive],"blue");

    //Transform/mesh/texture entities
    gea::Entity Terain = entityManager.createEntity();
    gea::Transform transform1;
    transform1.name = "Terrain";
    gea::Mesh mesh1;
    mesh1.path = "../../Assets/Models/output.obj";
    gea::Texture texture1;
    texture1.path = "../../Assets/Textures/viking_room.png";

    gea::Entity MainSphere = entityManager.createEntity();
    gea::Transform transform2;
    transform2.mPosition = glm::vec3(30.0,0.0,10.0); // initial position of the ball: Free to edit here
    transform2.name = "MainSphere";
    gea::Mesh mesh2 = sharedSphereMesh;
    gea::SphereCollision col1;
    gea::Texture texture2 = sharedSphereTexture;
    gea::Physics physics2;

    //Collision object setup.
    gea::Entity collisionObject = entityManager.createEntity();
    gea::Transform transform3;
    transform3.name = "collisionObject";
    transform3.mPosition = glm::vec3(4.0f, -11.0f, -18.0f);
    transform3.mScale = glm::vec3(5.0,5.0,5.0);
    gea::SphereCollision col2;
    col2.radius = 5;
    gea::Mesh mesh3 = sharedSphereMesh;
    gea::Texture texture3 = sharedSphereTexture;



    //Tracker
    gea::Entity tracker1 = entityManager.createEntity();
    gea::Tracker bug;
    gea::Transform transform4;
    transform4.name = "Main-Tracker";
    bug.enitityToTrack = MainSphere.mEntityID;
    bug.isTracking = true;


    //Fluid simulation - Comment out to test collision!      ###########################################
    totalBallsToSpawn = 50; //more than 150 seem to be too much
    for(int i = 0; i < totalBallsToSpawn; i++)
    {
        gea::Entity ball = entityManager.createEntity();

        gea::Transform transform;
        transform.mPosition = glm::vec3(-100.0,-100.0,-100.0); // spawn far away, fluidsystem will bring them back again.
        //transform.mScale = glm::vec3(0.5,0.5,0.5);
        transform.name = "FluidBall_" + std::to_string(ballsSpawned);

        gea::fluidSimComponent fluidSimComp;
        fluidSimComp.spawnPos = glm::vec3(30.0,0.0,10.0);

        gea::Physics physics;
        physics.collisionOn = false;

        gea::Entity fluidTracker = entityManager.createEntity();
        gea::Tracker tracker;
        tracker.enitityToTrack = ball.mEntityID;
        gea::Transform trackerTransform;
        trackerTransform.name = "Tracker_" + std::to_string(ballsSpawned);

        tracker.isTracking = true;


        registry.addComponent(ball.mEntityID, transform);
        registry.addComponent(ball.mEntityID, sharedSphereMesh);
        registry.addComponent(ball.mEntityID, sharedSphereTexture);
        registry.addComponent(ball.mEntityID, physics);
        registry.addComponent(ball.mEntityID, fluidSimComp);

        registry.addComponent(fluidTracker.mEntityID, tracker);
        registry.addComponent(fluidTracker.mEntityID, trackerTransform);

    }

    //###########################################################

    //connect components to mesh
    registry.addComponent(Terain.mEntityID, transform1);
    registry.addComponent(Terain.mEntityID, mesh1);
    registry.addComponent(Terain.mEntityID, texture1);
    registry.addComponent(Terain.mEntityID,col1);

    registry.addComponent(MainSphere.mEntityID, transform2);
    registry.addComponent(MainSphere.mEntityID, mesh2);
    registry.addComponent(MainSphere.mEntityID, texture2);
    registry.addComponent(MainSphere.mEntityID, physics2);

    registry.addComponent(collisionObject.mEntityID, col2);
    registry.addComponent(collisionObject.mEntityID, mesh3);
    registry.addComponent(collisionObject.mEntityID, texture3);
    registry.addComponent(collisionObject.mEntityID, transform3);


    //tracker components
    registry.addComponent(tracker1.mEntityID, transform4);
    registry.addComponent(tracker1.mEntityID, bug);

    // this has to happen last
    RenderSystem->init(renderSurface);
}

QString EngineInit::mat4ToQString(const glm::mat4& mat) {
    QStringList rows;
    for (int i = 0; i < 4; ++i) {
        QStringList rowElements;
        for (int j = 0; j < 4; ++j) {
            rowElements << QString::number(mat[i][j], 'f', 3);
        }
        rows << rowElements.join("\t");
    }
    return rows.join("\n");
}

void EngineInit::update()
{
    //Getting time passed between frames
    qint64 currentTime = timer.nsecsElapsed(); // time in nanoseconds
    float deltaTime = (currentTime - lastTime) / 1e9f; // convert to seconds
    lastTime = currentTime;

    InputSystem->update(deltaTime);
    fluidSystem->Update(deltaTime);
    CameraSystem->Update();
    PhysicsSystem->update(deltaTime);
    RenderSystem->Update(deltaTime);


}
} // End of namespace gea
