#include "PanoView360.h"

#include <cvrConfig/ConfigManager.h>
#include <cvrKernel/PluginHelper.h>
#include <cvrKernel/NodeMask.h>

#include <mxml.h>

#include <osg/Uniform>

#include <string>

using namespace cvr;
using namespace osg;
using namespace std;

static const string FILES("Plugin.PanoView360.Files");

CVRPLUGIN(PanoView360)

PanoView360* plugin = NULL;

PanoView360::PanoView360()
{

}

bool PanoView360::init()
{
    cerr << "PanoView360 init\n";

    plugin = this;

    _configFile = ConfigManager::getEntry("Plugin.PanoView360.ScreenConfig");

    //parseConfig(_configFile);

    _wasinit = 0;
    _deleteWait = false;
    _nextLoad = NULL;

    _cdLeft = NULL;
    _cdRight = NULL;
    _joystickSpin = ConfigManager::getBool("Plugin.PanoView360.JoystickSpin", true);
    _spinScale = ConfigManager::getFloat("Plugin.PanoView360.SpinScale",1.0);

    _root = new osg::MatrixTransform();

    float x,y,z;

    x = ConfigManager::getFloat("x","Plugin.PanoView360.Offset",0.0);
    y = ConfigManager::getFloat("y","Plugin.PanoView360.Offset",0.0);
    z = ConfigManager::getFloat("z","Plugin.PanoView360.Offset",0.0);

    osg::Matrix offset;
    offset.makeTranslate(osg::Vec3(x,y,z));
    _root->setMatrix(offset);

    osg::StateSet * stateset = _root->getOrCreateStateSet();

    osg::Uniform * light = new osg::Uniform();
    light->setName("MVLighting");
    light->setType(osg::Uniform::BOOL);
    light->set(false);
    stateset->addUniform(light);

    osg::Uniform * texture = new osg::Uniform();
    texture->setName("MVTexture");
    texture->setType(osg::Uniform::BOOL);
    texture->set(true);
    stateset->addUniform(texture);

    _panoViewMenu = new SubMenu("PanoView360","PanoView360");
    _panoViewMenu->setCallback(this);

    _loadMenu = new SubMenu("Load","Load");
    _loadMenu->setCallback(this);

    _panoViewMenu->addItem(_loadMenu);

    _tilesp = new MenuRangeValue("Segments Per Tile", 4, 150, 30);
    _tilesp->setCallback(this);
    _panoViewMenu->addItem(_tilesp);

    _radiusp = new MenuRangeValue("Radius", 10, 100000, 30000);
    _radiusp->setCallback(this);
    _panoViewMenu->addItem(_radiusp);

    _viewanglep = new MenuRangeValue("View Angle: V", 0, 180, 120);
    _viewanglep->setCallback(this);
    _panoViewMenu->addItem(_viewanglep);

    _viewanglepb = new MenuRangeValue("View Angle: H", 0, 360, 120);
    _viewanglepb->setCallback(this);
    _panoViewMenu->addItem(_viewanglepb);

    _camHeightp = new MenuRangeValue("Camera Height", -25000, 25000, 0);
    _camHeightp->setCallback(this);
    _panoViewMenu->addItem(_camHeightp);

    _remove = new MenuButton("Remove");
    _panoViewMenu->addItem(_remove);
    _remove->setCallback(this);


    std::vector<std::string> tagList;
    ConfigManager::getChildren(FILES, tagList);

    for(int i = 0; i < tagList.size(); i++)
    {
	std::string tag = FILES + "." + tagList[i];
	createLoadMenu(tagList[i], tag, _loadMenu);

	/*MenuButton* temp = new MenuButton(tagList[i]);
	temp->setCallback(this);
	_menufilelist.push_back(temp);
	struct loadinfo * info = new struct loadinfo;
	info->name = tagList[i];
	info->right_eye_file = ConfigManager::getEntry("reye", tag, "");
	info->left_eye_file = ConfigManager::getEntry("leye", tag, "");
	info->radius = ConfigManager::getFloat("radius", tag, 10000.0);
	info->viewanglev = ConfigManager::getFloat("viewanglev", tag, 120.0);
	info->viewangleh = ConfigManager::getFloat("viewangleh", tag, 360.0);
	info->camHeight = ConfigManager::getFloat("camHeight", tag, 0.0);
	info->segments = ConfigManager::getInt("segments", tag, 25);
	info->texture_size = ConfigManager::getInt("tsize", tag, 1024);
	info->flip = ConfigManager::getInt("flip", tag, 0);
	if(ConfigManager::getInt("sphere", tag, 0))
	{
	    info->shape = SPHERE;
	}
	else
	{
	    info->shape = CYLINDER;
	}

	_pictures.push_back(info);*/
    }
    
    //for(int i = 0; i < _menufilelist.size(); i++)
    //{
    //  _loadMenu->addItem(_menufilelist[i]);
    //}

    PluginHelper::addRootMenuItem(_panoViewMenu);
    PluginHelper::getScene()->addChild(_root);    

    cerr << "PanoView360 init done.\n";
    return true;
}


PanoView360::~PanoView360()
{
}

void PanoView360::menuCallback(MenuItem* menuItem)
{
    if(_deleteWait)
    {
	return;
    }
    if(menuItem == _remove)
    {
       // if(root->getNumChildren() != 0)
        //{
        //    root->removeChildren(0, root->getNumChildren());
        //}
	if(_cdLeft != NULL)
	{
	    _cdLeft->deleteTextures();
	    _cdRight->deleteTextures();
	    _deleteWait = true;
	    _nextLoad = NULL;
	}
	else
	{
	    if(_root->getNumChildren() != 0)
	    {
	        _root->removeChildren(0, _root->getNumChildren());
	    }
	}
        _wasinit = 0;
        return;
    }

    if(menuItem == _tilesp && _wasinit)
    {
        _cdLeft->setSegmentsPerTexture((int)_tilesp->getValue());
	_cdRight->setSegmentsPerTexture((int)_tilesp->getValue());
        return;
    }
    if(menuItem == _radiusp && _wasinit)
    {
        _cdLeft->setRadius((int)_radiusp->getValue());
	_cdRight->setRadius((int)_radiusp->getValue());
        return;
    }
    if((menuItem == _viewanglep || menuItem == _viewanglepb) && _wasinit)
    {
	_cdLeft->setViewAngle(_viewanglep->getValue(), _viewanglepb->getValue());
	_cdRight->setViewAngle(_viewanglep->getValue(), _viewanglepb->getValue());
	return;
    }

    if(menuItem == _camHeightp && _wasinit)
    {
	_cdLeft->setCamHeight(_camHeightp->getValue());
	_cdRight->setCamHeight(_camHeightp->getValue());
	return;
    }

    for(int i = 0; i < _menufilelist.size(); i++)
    {
        if(_menufilelist[i] == menuItem)
        {
            //if(root->getNumChildren() != 0)
            //{
                //root->removeChildren(0, root->getNumChildren());
            //}
	    if(_cdLeft != NULL)
	    {
		menuCallback(_remove);
		_nextLoad = menuItem;
		return;
	    }

	    switch(_pictures[i]->shape)
	    {
		case CYLINDER:
		{
		    _cdLeft = new CylinderDrawable(_pictures[i]->radius, _pictures[i]->viewanglev, _pictures[i]->viewangleh, _pictures[i]->camHeight, _pictures[i]->segments, _pictures[i]->texture_size);
		    //_cdLeft->setMap(_eyeMap);
		    _cdRight = new CylinderDrawable(_pictures[i]->radius, _pictures[i]->viewanglev, _pictures[i]->viewangleh, _pictures[i]->camHeight, _pictures[i]->segments, _pictures[i]->texture_size);
		    //_cdRight->setMap(_eyeMap);
		    break;
		}
		case SPHERE:
		{
		    _cdLeft = new SphereDrawable(_pictures[i]->radius, _pictures[i]->viewanglev, _pictures[i]->viewangleh, _pictures[i]->camHeight, _pictures[i]->segments, _pictures[i]->texture_size);
		    //_cdLeft->setMap(_eyeMap);
		    _cdRight = new SphereDrawable(_pictures[i]->radius, _pictures[i]->viewanglev, _pictures[i]->viewangleh, _pictures[i]->camHeight, _pictures[i]->segments, _pictures[i]->texture_size);
		    //_cdRight->setMap(_eyeMap);
		    break;
		}
		default:
		{
		    cerr << "PanoView360: Unknown shape." << endl;
		    break;
		}
	    }
            _cdLeft->setFlip(_pictures[i]->flip);
	    _cdRight->setFlip(_pictures[i]->flip);
            if(_pictures[i]->right_eye_file == "")
            {
                if(_pictures[i]->left_eye_file == "")
                {
                    cerr << "PanoView360: No files listed in config file for " << _pictures[i]->name << endl;
                    _cdLeft->unref();
		    _cdRight->unref();
                    return;
                }
                _cdLeft->setImage(_pictures[i]->left_eye_file);
		_cdRight->setImage(_pictures[i]->left_eye_file);
            }
            else if(_pictures[i]->left_eye_file == "")
            {
                if(_pictures[i]->right_eye_file == "")
                {
                    cerr << "PanoView360: No files listed in config file for " << _pictures[i]->name << endl;
                    _cdLeft->unref();
		    _cdRight->unref();
                    return;
                }
                _cdLeft->setImage(_pictures[i]->right_eye_file);
		_cdRight->setImage(_pictures[i]->right_eye_file);
            }
            else
            {
                _cdLeft->setImage(_pictures[i]->right_eye_file, _pictures[i]->left_eye_file);
		_cdRight->setImage(_pictures[i]->right_eye_file, _pictures[i]->left_eye_file);
            }
            _tilesp->setValue((float)_cdLeft->getSegmentsPerTexture());
            _radiusp->setValue((float)_cdLeft->getRadius());
	    float a, b;
	    _cdLeft->getViewAngle(a, b);
            _viewanglep->setValue(a);
	    _viewanglepb->setValue(b);
            _camHeightp->setValue(_cdLeft->getCamHeight());
            osg::Geode * geo = new osg::Geode();
	    geo->setNodeMask(geo->getNodeMask() & ~(CULL_MASK_RIGHT));
            geo->addDrawable(_cdLeft);
            _root->addChild(geo);

	    geo = new osg::Geode();
	    geo->setNodeMask(geo->getNodeMask() & ~(CULL_MASK_LEFT));
	    geo->setNodeMask(geo->getNodeMask() & ~(CULL_MASK));
            geo->addDrawable(_cdRight);
            _root->addChild(geo);
	    _root->setNodeMask(_root->getNodeMask() & ~(CULL_ENABLE));

            _wasinit = 1;
        }
    }
}

/// Called before each frame
void PanoView360::preFrame()
{
    if(_deleteWait)
    {
	if(_cdLeft->deleteDone() && _cdRight->deleteDone())
	{
	    if(_root->getNumChildren() != 0)
	    {
	        _root->removeChildren(0, _root->getNumChildren());
	    }
	    _cdLeft = NULL;
	    _cdRight = NULL;

	    _deleteWait = false;
	    if(_nextLoad)
	    {
		menuCallback(_nextLoad);
	    }
	}
    }
   if(_cdLeft != NULL && _joystickSpin)
   {
       _cdLeft->updateRotate(_spinScale * PluginHelper::getValuator(0,0));
       _cdRight->updateRotate(_spinScale * PluginHelper::getValuator(0,0));
   } 
    if (_cdLeft) {
        osg::Matrixd camera = PluginHelper::getObjectMatrix();
        osg::Quat qcamera = camera.getRotate();
        osg::Matrixd newcamera;
        newcamera.makeIdentity();
        #if 0
        {
                osg::Quat newqcamera;
                // Limit angular degrees of freedom...
                // See also: http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
                float q0 = qcamera.x();
                float q1 = qcamera.y();
                float q2 = qcamera.z();
                float q3 = qcamera.w();
                float t1 = atan2(2*(q0*q1 + q2*q3), 1 - 2*(q1*q1 + q2*q2));
                float t2 = 0; //asin(2*(q0*q2 - q3*q1));
                float t3 = atan2(2*(q0*q3 + q1*q2), 1-2*(q2*q2 + q3*q3));
                newqcamera.set(
                    cos(t1/2)*cos(t2/2)*cos(t3/2) + sin(t1/2)*sin(t2/2)*sin(t3/2),
                    sin(t1/2)*cos(t2/2)*cos(t3/2) - cos(t1/2)*sin(t2/2)*sin(t3/2),
                    cos(t1/2)*sin(t2/2)*cos(t3/2) + sin(t1/2)*cos(t2/2)*sin(t3/2),
                    cos(t1/2)*cos(t2/2)*sin(t3/2) - sin(t1/2)*sin(t2/2)*cos(t3/2)
                );
                newcamera.postMultRotate(newqcamera);
        }
        #endif
        {
            float x = camera(0, 0);
            float y = camera(1, 0);
            float z = camera(2, 0);
            //float scale; // TODO
            float azimuth = atan2(y, x);
            //float elevation = atan2(z, sqrt(x*x + y*y)); // <-- not quite right, yet
            //newcamera *= osg::Matrixd::rotate(azimuth, 0, 0, 1);
            //newcamera *= osg::Matrixd::rotate(elevation, 1, 0, 0); // <-- not quite right, yet
            _cdLeft->updateRotate(azimuth);
            _cdRight->updateRotate(azimuth);
        }
    }
}

bool PanoView360::processEvent(InteractionEvent * event)
{
    if(!event->asKeyboardEvent())
    {
	return false;
    }
    if(_cdLeft != NULL)
    {
	_cdLeft->updateRotate(0.6);
	_cdRight->updateRotate(0.6);
    }
    return false;
}

/*void PanoView360::parseConfig(std::string file)
{
    FILE * fp;
    mxml_node_t * tree;

    //cerr << "Reading file: " << file << endl;

    fp = fopen(file.c_str(), "r");
    if(fp == NULL)
    {
	cerr << "Unable to open file: " << file << std::endl;
	return;
    }

    tree = mxmlLoadFile(NULL, fp,
	    MXML_TEXT_CALLBACK);
    fclose(fp);

    if(tree == NULL)
    {
	cerr << "Unable to parse XML file: " << file << std::endl;
	return;
    }

    mxml_node_t *node;

    for (node = mxmlFindElement(tree, tree, "screen", NULL, NULL, MXML_DESCEND); node != NULL; node = mxmlFindElement(node, tree, "screen", NULL, NULL, MXML_DESCEND))
    {
	int vx, vy, context;
	string host, eye;

	vx = atoi(mxmlElementGetAttr(node, "viewportx"));
	vy = atoi(mxmlElementGetAttr(node, "viewporty"));
	context = atoi(mxmlElementGetAttr(node, "context"));
	host = mxmlElementGetAttr(node, "host");
	eye = mxmlElementGetAttr(node, "eye");

	//cerr << "Entry: host: " << host << " vx: " << vx << " vy: " << vy << " context: " << context << " eye: " << eye << endl;

	
	int eyei = 0;
	
	if(eye == "RIGHT")
	{
	    eyei = 1;
	}
	else if(eye == "LEFT")
	{
	    eyei = 2;
	}
	else if(eye == "BOTH")
	{
	    PanoDrawable::firsteye = atoi(mxmlElementGetAttr(node, "firsteye"));
	    eyei = 3;
	}
	else
	{
	    cerr << "Invalid eye entry." << endl;
	}

	_eyeMap[host][context].push_back(pair<pair<int, int>, int >(pair<int, int>(vx, vy), eyei));
    }
}*/

void PanoView360::createLoadMenu(std::string tagBase, std::string tag, SubMenu * menu)
{
    std::vector<std::string> tagList;
    ConfigManager::getChildren(tag, tagList);

    if(tagList.size())
    {
	SubMenu * sm = new SubMenu(tagBase);
	menu->addItem(sm);
	for(int i = 0; i < tagList.size(); i++)
	{
	    createLoadMenu(tagList[i], tag + "." + tagList[i], sm);
	}
    }
    else
    {
	MenuButton* temp = new MenuButton(tagBase);
	temp->setCallback(this);
	_menufilelist.push_back(temp);
	struct loadinfo * info = new struct loadinfo;
	info->name = tagBase;
	info->right_eye_file = ConfigManager::getEntry("reye", tag, "");
	info->left_eye_file = ConfigManager::getEntry("leye", tag, "");
	info->radius = ConfigManager::getFloat("radius", tag, 10000.0);
	info->viewanglev = ConfigManager::getFloat("viewanglev", tag, 120.0);
	info->viewangleh = ConfigManager::getFloat("viewangleh", tag, 360.0);
	info->camHeight = ConfigManager::getFloat("camHeight", tag, 0.0);
	info->segments = ConfigManager::getInt("segments", tag, 25);
	info->texture_size = ConfigManager::getInt("tsize", tag, 1024);
	info->flip = ConfigManager::getInt("flip", tag, 0);
	if(ConfigManager::getInt("sphere", tag, 0))
	{
	    info->shape = SPHERE;
	}
	else
	{
	    info->shape = CYLINDER;
	}

	_pictures.push_back(info);
	menu->addItem(temp);
    }
}
