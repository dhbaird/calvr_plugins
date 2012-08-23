#ifndef ELEVATORROOM_PLUGIN_H
#define ELEVATORROOM_PLUGIN_H


#include <cvrKernel/CVRPlugin.h>
#include <cvrKernel/FileHandler.h>

#include <cvrMenu/SubMenu.h>
#include <cvrMenu/MenuRangeValue.h>
#include <cvrMenu/MenuCheckbox.h>
#include <cvrMenu/MenuButton.h>
#include <cvrMenu/MenuText.h>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>
#include <osg/Switch>
#include <osg/Texture2D>
#include <osgText/Text>

#include <string.h>
#include <vector>
#include <map>

#include "OAS/OASClient.h"

#define NUM_DOORS 8
#define DOOR_SPEED 0.007
#define FLASH_SPEED 4
#define NUM_ALLY_FLASH 3
#define NUM_ALIEN_FLASH 8
#define LIGHT_PAUSE_LENGTH 4

class ElevatorRoom: public cvr::CVRPlugin, public cvr::MenuCallback
{
    public:
        ElevatorRoom();
        virtual ~ElevatorRoom();
        static ElevatorRoom * instance();
        bool init();
        void menuCallback(cvr::MenuItem * item);
        void preFrame();
        bool processEvent(cvr::InteractionEvent * event);

    protected:
        static ElevatorRoom * _myPtr;

        enum Mode
        {
            NONE,
            ALIEN,
            ALLY,
            CHECKER
        };

        cvr::SubMenu * _elevatorMenu;
        cvr::MenuButton * _loadButton, * _clearButton;
        cvr::MenuRangeValue * _checkerSpeedRV, * _alienChanceRV;
        cvr::MenuText * _chancesText;

        osg::ref_ptr<osg::MatrixTransform> _geoRoot; // root of all non-GUI plugin geometry
        std::string _dataDir;
            
        osg::ref_ptr<osg::Geode> _activeObject; // currently active avatar
        osg::ref_ptr<osgText::Text> _scoreText; // GUI to display current score
        
        std::vector<osg::ref_ptr<osg::PositionAttitudeTransform> > _leftdoorPat,    
            _rightdoorPat;
        std::vector<osg::ref_ptr<osg::ShapeDrawable> > _lights;

        // first node is regular geometry, second node is flashing geometry
        std::vector<osg::ref_ptr<osg::Switch> > _aliensSwitch, _alliesSwitch, 
            _checkersSwitch, _lightSwitch;
        
        oasclient::Sound * _ding, * _hitSound, * _laser;

        float _modelScale; // scale of entire scene
        float _pauseLength; // length in seconds of time between door close and next lighting up
        float _pauseStart; // start time of the current pause
        float _doorDist; // distance doors are currently translated
        float _checkSpeed; // number of checkerboard flashes per second
        float _flashStartTime;
        float _avatarFlashPerSec;
        float _lightFlashPerSec;

        int _activeDoor; // which door is currently opening/closing
        int _score; // current score (should be > 0)
        int _sockfd; // for EOG syncer communication
        int _flashCount; // number of times active avatar has flashed
        int _alienChance, _allyChance, _checkChance;

        bool _isOpening; // whether the active door is opening or closing
        bool _loaded; // whether the model has finished loading
        bool _hit; // whether a hit has been made on the active avatar
        bool _debug; // turns on debug messages to command line
        bool _connected; // for EOG syncer communication
        bool _soundEnabled;

        Mode _mode; // which kind of avatar is currently active
        
        void connectToServer();
        void openDoor(int doorNum);
        void closeDoor(int doorNum);
        void loadModels();
        void clear();
        osg::ref_ptr<osg::Geometry> drawBox(osg::Vec3 center, float x, float y,
            float z, osg::Vec4 color = osg::Vec4(1, 1, 1, 1), float texScale = 1.0);
        osg::ref_ptr<osg::Geometry> makeQuad(float width, float height,
            osg::Vec4 color, osg::Vec3 pos);

};

#endif
