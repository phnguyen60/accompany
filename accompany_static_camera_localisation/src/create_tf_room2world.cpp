#include <opencv2/opencv.hpp>
#include <iostream>
#include <boost/program_options.hpp>
#include <yaml-cpp/yaml.h>

#include <ros/ros.h>
#include <geometry_msgs/TransformStamped.h>
#include <tf/transform_datatypes.h>
#include <accompany_uva_utils/uva_utils.h>

namespace po = boost::program_options;
using namespace std;
using namespace cv;

Scalar BLUE = CV_RGB(0,0,255), GREEN = CV_RGB(0,255,0), RED = CV_RGB(255,0,0);
vector<Scalar> COLORS;
string text = "123";
float scale = 1;
const unsigned int NUM_PNTS = 3;
vector<Point> locations;
Mat img;

void showScaledImg()
{
  Mat scaled;
  Point pnt(10, 10);
  Point pnt2(-5, 15);
  Size dst_sz(round(scale * img.cols), round(scale * img.rows));
  resize(img, scaled, dst_sz, 0, 0);
  for (unsigned int i = 0; i < locations.size(); ++i)
  {
    circle(scaled, locations[i] * scale, 2, COLORS[i], 1);
    rectangle(scaled, (locations[i] - pnt) * scale,
        (locations[i] + pnt) * scale, COLORS[i], 2 * scale);
    putText(scaled, text.substr(i, 1), (locations[i] - pnt2) * scale,
        FONT_HERSHEY_SIMPLEX, 0.5 * scale, COLORS[i], 2 * scale);
  }
  imshow("image", scaled);
}

void mouseHandler(int event, int x, int y, int flags, void *param)
{

  Point p = Point(x, y);
  if (event == CV_EVENT_LBUTTONDOWN && locations.size() < NUM_PNTS)
  {
      p *= 1 / scale;
      cout << "Left button down at " << p.x << "," << p.y << endl;
      locations.push_back(p);
      showScaledImg();
  }
}

void printUsage()
{
  cout << "Options:" << endl;
  cout << "  " << "LEFT Mouse   " << "select points" << endl;
  cout << "  " << "+" << "      " << "zoom in" << endl;
  cout << "  " << "-" << "      " << "zoom out" << endl;
  cout << "  " << "z" << "      " << "step back" << endl;
  cout << "  " << "q" << "  " << "save&quit" << endl;
}

int main(int argc, char **argv)
{
  string imFile, mapParamFile, name;

  // handling arguments
  po::options_description optionsDescription(
      "Find tf from room coordinates to world coordinates\nAllowed options\n");
  optionsDescription.add_options()
    ("map,m",po::value<string>(&imFile)->required(), "the world map\n")
    ("param,p",po::value<string>(&mapParamFile)->required(), "parameters of the map\n")
    ("name,o",po::value<string>(&name)->required(), "name of the output tf file (.dat)\n");

  po::variables_map variablesMap;

  try
  {
    po::store(po::parse_command_line(argc, argv, optionsDescription),
        variablesMap);
    po::notify(variablesMap);
  }
  catch (const std::exception& e)
  {
    std::cout << "--------------------" << std::endl;
    std::cerr << "- " << e.what() << std::endl;
    std::cout << "--------------------" << std::endl;
    std::cout << optionsDescription << std::endl;
    return 1;
  }

  YAML::Node mapNode = YAML::LoadFile(mapParamFile);
  float resolution = mapNode["resolution"].as<float>();
  vector<float> origin; 
  YAML::Node originNode = mapNode["origin"];
  origin.push_back(originNode[0].as<float>());
  origin.push_back(originNode[1].as<float>());
  origin.push_back(originNode[2].as<float>());
  
  cout << "origin is: " << origin[0] << ", " << origin[1] << ", "  << origin[2] << endl;
  cout << "resolution is: " << resolution << endl;

  img = imread(imFile);
  namedWindow("image");
  setMouseCallback("image", mouseHandler, NULL);
  imshow("image", img);

  COLORS.push_back(BLUE);
  COLORS.push_back(RED);
  COLORS.push_back(GREEN);

  int key = 0;
  while (true)
  {
    if (locations.size() == 0)
    {
      printUsage();
    }
    if (locations.size() == NUM_PNTS) // exit
      break;
    
    key = waitKey(0);

    switch ((char) key)
    {
      case '-':
        cout << "zoom out" << endl;
        if (scale > .1)
          scale -= .1;
        showScaledImg();
        break;

      case '+':
        cout << "zoom in" << endl;
        if (scale < 4)
          scale += .1;
        showScaledImg();
        break;

      case 'z':
        cout << "step back" << endl;
        if (locations.size())
          locations.pop_back();
        showScaledImg();
        break;

      default:
        printUsage();
        break;
    }
  }

  Mat src_points = Mat::zeros(Size(2, 3), CV_32F);
  for (unsigned int i = 0; i < locations.size(); ++i)
  {
    src_points.at<float>(i, 0) = locations[i].x * resolution + origin[0];
    src_points.at<float>(i, 1) = (img.rows - locations[i].y) * resolution + origin[1];
  }

  cout << "src_points are: " << src_points << endl;
  cout << "-------------------------------------" << endl;

  Mat dst_points = Mat::zeros(Size(2, 3), CV_32F);
  cout << "input the world coordinates of point " << text[0] << ", separate with SPACE" << endl;
  cin >> dst_points.at<float>(0, 0) >> dst_points.at<float>(0, 1);
  cout << "input the world coordinates of point " << text[1] << ", separate with SPACE" << endl;
  cin >> dst_points.at<float>(1, 0) >> dst_points.at<float>(1, 1);
    cout << "input the world coordinates of point " << text[2] << ", separate with SPACE" << endl;
  cin >> dst_points.at<float>(2, 0) >> dst_points.at<float>(2, 1);
//  cout << "input the world coordinates of 'Y', separate with SPACE" << endl;
//  dst_points.at<float>(2, 0) = dst_points.at<float>(1, 1) * (-1);
//  dst_points.at<float>(2, 1) = dst_points.at<float>(1, 0);
  cout << "dst_points are: " << dst_points << endl;
  cout << "-------------------------------------" << endl;

  Mat tform( 2, 3, CV_32FC1 );;
  tform = getAffineTransform(src_points, dst_points);
  cout << "src_points: " << endl << src_points << endl;
  cout << "dst_points: " << endl << dst_points << endl;
  cout << "transform matrix: " << endl << tform << endl;
  cout << "How it works: " << endl
      << "dst_points = transform matrix * [src_points'; 1,1,1]" << endl;

  string filename="frame.dat";
  cout<<"create some frame and write to file '"<<filename<<"'"<<endl;
  geometry_msgs::TransformStamped transformStamped;
  tf::Transform transform = tf::Transform(
        btMatrix3x3(tform.at<double>(0,0),tform.at<double>(0,1),0,// rotation matrix
                  tform.at<double>(1,0),tform.at<double>(1,1),0,
                  0,0,1), 
        btVector3(tform.at<double>(0,2),tform.at<double>(1,2),0));// translation vector
  
  tf::Matrix3x3 ma = transform.getBasis();
  cout << ma.getRow(0)[0] << "," << ma.getRow(0)[1] << "," << ma.getRow(0)[2] << endl;
  cout << ma.getRow(1)[0] << "," << ma.getRow(1)[1] << "," << ma.getRow(1)[2] << endl;
  cout << ma.getRow(2)[0] << "," << ma.getRow(2)[1] << "," << ma.getRow(2)[2] << endl;
  
  tf::StampedTransform stampedTransform=tf::StampedTransform(
        transform,     // the transform
        ros::Time(0),  // time, not used here
        "/map",        // parent coordinate frame
        name); // child coordinate frame
  tf::transformStampedTFToMsg(stampedTransform,transformStamped);
  save_msg(transformStamped,filename); // write to file
 

  cout<<"read from file '"<<filename<<"' and print, just a test:"<<endl;
  geometry_msgs::TransformStamped transformStamped2;
  load_msg(transformStamped2,filename);
  cout << transformStamped2;
  
  waitKey(0);
  return 0;
}
