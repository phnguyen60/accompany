#include "CamCalib.hh"
#include <string>
#include <data/XmlFile.hh>
#include <fstream>
#include <tools/string.hh>
#include <err.h>
#include "Helpers.hh"
#include <iostream>
// #include
// // #include <opencv/cv.h>

using namespace std;

std::vector<CamCalib> cam;
double personHeight = 0, wg = 0, wm = 0, wt = 0, midRatio = 0;
double minX, maxX, minY, maxY;
string ExtrinsicFile, IntrinsicFile;

#define OCTAGON 1

double sqGroundDist(const WorldPoint &p1, const WorldPoint &p2)
{
  double dx = p1.x - p2.x, dy = p1.y - p2.y; // ignore z

  return dx * dx + dy * dy;
}

ostream &operator<<(ostream &os, const WorldPoint &wp)
{
  return os << "(" << wp.x << "," << wp.y << "," << wp.z << ")";
}

CamCalib::CamCalib() :
    stdev1(9), stdev2(9), stdev3(9), scale(1.0)
{
  updateCache();
}

void CamCalib::xmlPack(XmlFile &f) const
{
  //     string modelFile; // TODO
  //     f.pack("ModelFile", modelFile);
  // f.pack("w", w);
  // f.pack("w2", w2);
  f.pack("sigma1", stdev1);
  f.pack("sigma2", stdev2);
  f.pack("sigma3", stdev3);
  f.pack("scale", scale);
}

void CamCalib::xmlUnpack(XmlFile &f)
{
  double scale;
  f.unpack("SCALE", scale);

  ifstream ifs(ExtrinsicFile.c_str());
  if (!ifs)
  {
    cerr << "Could not load file " << ExtrinsicFile << endl;
    exit(1);
  }
  
  // initialize camera model 
  model.init(IntrinsicFile, ExtrinsicFile, scale);
  cout << "SCALE" << "," << scale << endl;
  // f.unpack("w", w);
  // f.unpack("w2", w2);
  f.unpack("sigma1", stdev1);
  f.unpack("sigma2", stdev2);
  f.unpack("sigma3", stdev3);
  scale = f.unpackDflt("scale", scale);

  updateCache();
}

#define UNDISTORT 0
WorldPoint CamCalib::getGroundPos(const CvPoint &pt) const
{
  WorldPoint res;
  res.z = 0;

#if UNDISTORT
  cout << "UNDISTORT deprecated" << endl;
  exit(0);
  //     double x = (double)pt.x/scale, y=(double)pt.y/scale, xu, yu;
  //     const_cast<Hu::CameraModel*>(&model)->distortedToUndistortedImageCoord(x,y,xu,yu);
  //     const_cast<Hu::CameraModel*>(&model)->imageToWorld(xu,yu,res.z,res.x,res.y);
  //     return res;
#else
  //     double x = (double)pt.x/scale, y=(double)pt.y/scale; // TODO what is scale??
  double x = (double) pt.x, y = (double) pt.y; // TODO what is scale??
  const_cast<Hu::CameraModel*>(&model)->imageToWorld(x, y, res.z, res.x, res.y);
  return res;
#endif
}

CvPoint CamCalib::project(const WorldPoint &pt) const
{
#if UNDISTORT
  cout << "UNDISTORT deprecated" << endl;
  exit(0);
  //     double x,y;
  //     double xd, yd;
  //     const_cast<Hu::CameraModel*>(&model)->worldToImage(pt.x,pt.y,pt.z,x,y);
  //     const_cast<Hu::CameraModel*>(&model)->undistortedToDistortedImageCoord(x,y,xd,yd);
  //     return cvPoint(xd*scale,yd*scale);
  //     return cvPoint(xd,yd);
#else
  double x, y;
  const_cast<Hu::CameraModel*>(&model)->worldToImage(pt.x, pt.y, pt.z, x, y);
  return cvPoint(x * scale, y * scale);
#endif
}

#if OCTAGON
void CamCalib::genTemplate(const WorldPoint &pt, vector<CvPoint> &points) const
{
  double x = pt.x, y = pt.y, hw = wg / 2, mh = midRatio * personHeight, s45 =
      sqrt(2.) / 2., shw = s45 * hw;

  points.resize(24);
  points[0] = project(WorldPoint(x + hw, y));
  points[1] = project(WorldPoint(x + shw, y + shw));
  points[2] = project(WorldPoint(x, y + hw));
  points[3] = project(WorldPoint(x - shw, y + shw));
  points[4] = project(WorldPoint(x - hw, y));
  points[5] = project(WorldPoint(x - shw, y - shw));
  points[6] = project(WorldPoint(x, y - hw));
  points[7] = project(WorldPoint(x + shw, y - shw));

  hw = wm / 2;
  shw = s45 * hw;
  points[8] = project(WorldPoint(x + hw, y, mh));
  points[9] = project(WorldPoint(x + shw, y + shw, mh));
  points[10] = project(WorldPoint(x, y + hw, mh));
  points[11] = project(WorldPoint(x - shw, y + shw, mh));
  points[12] = project(WorldPoint(x - hw, y, mh));
  points[13] = project(WorldPoint(x - shw, y - shw, mh));
  points[14] = project(WorldPoint(x, y - hw, mh));
  points[15] = project(WorldPoint(x + shw, y - shw, mh));

  // points[4] = project(WorldPoint(x-hw,y-hw,mh));
  // points[5] = project(WorldPoint(x-hw,y+hw,mh));
  // points[6] = project(WorldPoint(x+hw,y+hw,mh));
  // points[7] = project(WorldPoint(x+hw,y-hw,mh));

  hw = wt / 2;
  shw = s45 * hw;
  points[16] = project(WorldPoint(x + hw, y, personHeight));
  points[17] = project(WorldPoint(x + shw, y + shw, personHeight));
  points[18] = project(WorldPoint(x, y + hw, personHeight));
  points[19] = project(WorldPoint(x - shw, y + shw, personHeight));
  points[20] = project(WorldPoint(x - hw, y, personHeight));
  points[21] = project(WorldPoint(x - shw, y - shw, personHeight));
  points[22] = project(WorldPoint(x, y - hw, personHeight));
  points[23] = project(WorldPoint(x + shw, y - shw, personHeight));

  // points[8] = project(WorldPoint(x-hw,y-hw,personHeight));
  // points[9] = project(WorldPoint(x-hw,y+hw,personHeight));
  // points[10] = project(WorldPoint(x+hw,y+hw,personHeight));
  // points[11] = project(WorldPoint(x+hw,y-hw,personHeight));
}
#else
void CamCalib::genTemplate(const WorldPoint &pt, vector<CvPoint> &points) const
{
  double x = pt.x, y = pt.y, hw = wg/2, mh = midRatio*personHeight, s45=sqrt(2.)/2., shw=s45*hw;

  // points.resize(24);
  points.resize(12);
  points[0] = project(WorldPoint(x+hw,y));
  points[1] = project(WorldPoint(x+shw,y+shw));
  points[2] = project(WorldPoint(x,y+hw));
  points[3] = project(WorldPoint(x-shw,y+shw));
  // points[4] = project(WorldPoint(x-hw,y));
  // points[5] = project(WorldPoint(x-shw,y-shw));
  // points[6] = project(WorldPoint(x,y-hw));
  // points[7] = project(WorldPoint(x+shw,y-shw));

  hw = wm/2;
  // shw = s45*hw;
  // points[8] = project(WorldPoint(x+hw,y ,mh));
  // points[9] = project(WorldPoint(x+shw,y+shw,mh));
  // points[10] = project(WorldPoint(x,y+hw,mh));
  // points[11] = project(WorldPoint(x-shw,y+shw,mh));
  // points[12] = project(WorldPoint(x-hw,y,mh));
  // points[13] = project(WorldPoint(x-shw,y-shw,mh));
  // points[14] = project(WorldPoint(x,y-hw,mh));
  // points[15] = project(WorldPoint(x+shw,y-shw,mh));

  points[4] = project(WorldPoint(x-hw,y-hw,mh));
  points[5] = project(WorldPoint(x-hw,y+hw,mh));
  points[6] = project(WorldPoint(x+hw,y+hw,mh));
  points[7] = project(WorldPoint(x+hw,y-hw,mh));

  hw = wt/2;
  // shw = s45*hw;
  // points[16] = project(WorldPoint(x+hw,y ,personHeight));
  // points[17] = project(WorldPoint(x+shw,y+shw,personHeight));
  // points[18] = project(WorldPoint(x,y+hw,personHeight));
  // points[19] = project(WorldPoint(x-shw,y+shw,personHeight));
  // points[20] = project(WorldPoint(x-hw,y,personHeight));
  // points[21] = project(WorldPoint(x-shw,y-shw,personHeight));
  // points[22] = project(WorldPoint(x,y-hw,personHeight));
  // points[23] = project(WorldPoint(x+shw,y-shw,personHeight));

  points[8] = project(WorldPoint(x-hw,y-hw,personHeight));
  points[9] = project(WorldPoint(x-hw,y+hw,personHeight));
  points[10] = project(WorldPoint(x+hw,y+hw,personHeight));
  points[11] = project(WorldPoint(x+hw,y-hw,personHeight));
}
#endif
/**
 * \brief Generate a rectangular template
 * \param pt The position in world coordinates
 * \param points The corner points of the template, in pixel coordinates
 *
 * 2011/03/07: GWENN - First version
 *
 **/
void CamCalib::genFleuretTemplate(const WorldPoint &pt,
    vector<CvPoint> &points) const
{
  double x = pt.x, y = pt.y, ratio = personHeight / wg;

  CvPoint midGround = project(WorldPoint(x, y)), midTop = project(
      WorldPoint(x, y, personHeight));

  int pixelWidth = int((midTop.y - midGround.y) / ratio);
  points.resize(4);
  points[0] = cvPoint(midGround.x - pixelWidth, midGround.y);
  points[1] = cvPoint(midGround.x + pixelWidth, midGround.y);
  points[2] = cvPoint(midGround.x - pixelWidth, midTop.y);
  points[3] = cvPoint(midGround.x + pixelWidth, midTop.y);
}

FLOAT logGaus(FLOAT sqDiff, FLOAT lpv2, FLOAT pv2)
{
  return -lpv2 - sqDiff / pv2;
}

void CamCalib::computeBGProb(const vnl_vector<FLOAT> &img,
    const vnl_vector<FLOAT> &bg, vnl_vector<FLOAT> &sumPixelProb, FLOAT &total)
{
  // Compute p(img|bg)
  vnl_vector<FLOAT> diff = img - bg;
  diff = element_product(diff, diff); // squared difference from mean

  if (sumPixelProb.size() != (width + 1) * height)
    sumPixelProb.set_size((width + 1) * height);

  total = 0.0;
  for (unsigned i = 0, j = 0; i != sumPixelProb.size(); ++i)
  {
    if (i % (width + 1) == 0)
    {
      sumPixelProb(i) = 0.0;
    }
    else
    {
      FLOAT lProb = logGaus(diff(j), lpv2_1, pv2_1)
          + logGaus(diff(j + 1), lpv2_2, pv2_2)
          + logGaus(diff(j + 2), lpv2_3, pv2_3);
      sumPixelProb(i) = sumPixelProb(i - 1) + lProb;
      total += lProb;
      j += 3;
    }
  }
}

void CamCalib::computeBGProbDiff(const vnl_vector<FLOAT> &img,
    const vnl_vector<FLOAT> &bg, vnl_vector<FLOAT> &sumPixelProb, FLOAT &total)
{
  // Compute p(img|bg)
  vnl_vector<FLOAT> diff = img - bg;
  diff = element_product(diff, diff); // squared difference from mean

  if (sumPixelProb.size() != (width + 1) * height)
    sumPixelProb.set_size((width + 1) * height);

  total = 0.0;
  for (unsigned i = 0, j = 0; i != sumPixelProb.size(); ++i)
  {
    if (i % (width + 1) == 0)
    {
      sumPixelProb(i) = 0.0;
    }
    else
    {
      FLOAT lProb = logGaus(diff(j), lpv2_1, pv2_1)
          + logGaus(diff(j + 1), lpv2_2, pv2_2)
          + logGaus(diff(j + 2), lpv2_3, pv2_3);
      sumPixelProb(i) = sumPixelProb(i - 1) + lProb + 3.0 * log(256); // - (-log ...)
      total += lProb;
      j += 3;
    }
  }
}

void CamCalib::computeDiff(const vnl_vector<FLOAT> &img,
    const vnl_vector<FLOAT> &bg, vnl_vector<FLOAT> &diffImg)
{
  // Compute p(img|bg)
  vnl_vector<FLOAT> diff = img - bg;
  diff = element_product(diff, diff); // squared difference from mean

  if (diffImg.size() != width * height)
    diffImg.set_size(width * height);

  for (unsigned i = 0, j = 0; i != diffImg.size(); ++i)
  {
    FLOAT lProb = logGaus(diff(j), lpv2_1, pv2_1)
        + logGaus(diff(j + 1), lpv2_2, pv2_2)
        + logGaus(diff(j + 2), lpv2_3, pv2_3);
    diffImg(i) = 255 * (lProb < -20);
    j += 3;
  }
}

/*******************************************************************************
 *                            Global functions
 *******************************************************************************/

#if !OCTAGON
void plotTemplate(IplImage *img, const vector<CvPoint> &points,
    const CvScalar &colour, unsigned lw)
{
  vector<CvPoint> hull;
  convexHull(points,hull);

  // CvPoint * corners = new CvPoint[hull.size()];
  // for (unsigned i=0; i!=hull.size(); ++i)
  //      corners[i] = hull[i];
  // cvFillConvexPoly(img, corners, hull.size(), CV_RGB(255,255,255));

  cvLine(img, points[0], points[1], colour,lw);
  cvLine(img, points[1], points[2], colour,lw);
  cvLine(img, points[2], points[3], colour,lw);
  cvLine(img, points[3], points[0], colour,lw);

  cvLine(img, points[4], points[5], colour,lw);
  cvLine(img, points[5], points[6], colour,lw);
  cvLine(img, points[6], points[7], colour,lw);
  cvLine(img, points[7], points[4], colour,lw);

  cvLine(img, points[8], points[9], colour,lw);
  cvLine(img, points[9], points[10], colour,lw);
  cvLine(img, points[10], points[11], colour,lw);
  cvLine(img, points[11], points[8], colour,lw);

  cvLine(img, points[0], points[4], colour,lw);
  cvLine(img, points[1], points[5], colour,lw);
  cvLine(img, points[2], points[6], colour,lw);
  cvLine(img, points[3], points[7], colour,lw);

  cvLine(img, points[4], points[8], colour,lw);
  cvLine(img, points[5], points[9], colour,lw);
  cvLine(img, points[6], points[10], colour,lw);
  cvLine(img, points[7], points[11], colour,lw);

  // for (unsigned i=1; i<hull.size(); ++i)
  //      cvLine(img,hull[i-1],hull[i],colour,lw+1);
  // cvLine(img,hull.back(),hull.front(),colour,lw+1);
  // points[0] = project(WorldPoint(x+hw,y));
  // points[1] = project(WorldPoint(x+shw,y+shw));
  // points[2] = project(WorldPoint(x,y+hw));
  // points[3] = project(WorldPoint(x-shw,y+shw));
  // points[4] = project(WorldPoint(x-hw,y));
  // points[5] = project(WorldPoint(x-shw,y-shw));
  // points[6] = project(WorldPoint(x,y-hw));
  // points[7] = project(WorldPoint(x+shw,y-shw));

  // hw = wm/2;
  // shw = s45*hw;
  // points[8] = project(WorldPoint(x+hw,y ,mh));
  // points[9] = project(WorldPoint(x+shw,y+shw,mh));
  // points[10] = project(WorldPoint(x,y+hw,mh));
  // points[11] = project(WorldPoint(x-shw,y+shw,mh));
  // points[12] = project(WorldPoint(x-hw,y,mh));
  // points[13] = project(WorldPoint(x-shw,y-shw,mh));
  // points[14] = project(WorldPoint(x,y-hw,mh));
  // points[15] = project(WorldPoint(x+shw,y-shw,mh));

  // hw = wt/2;
  // shw = s45*hw;
  // points[16] = project(WorldPoint(x+hw,y ,personHeight));
  // points[17] = project(WorldPoint(x+shw,y+shw,personHeight));
  // points[18] = project(WorldPoint(x,y+hw,personHeight));
  // points[19] = project(WorldPoint(x-shw,y+shw,personHeight));
  // points[20] = project(WorldPoint(x-hw,y,personHeight));
  // points[21] = project(WorldPoint(x-shw,y-shw,personHeight));
  // points[22] = project(WorldPoint(x,y-hw,personHeight));
  // points[23] = project(WorldPoint(x+shw,y-shw,personHeight));

}

#else  // OCTAGON
void plotTemplate(IplImage *img, const vector<CvPoint> &points,
    const CvScalar &colour, unsigned lw)
{
  // vector<CvPoint> hull;
  // convexHull(points,hull);

  // // CvPoint * corners = new CvPoint[hull.size()];
  // // for (unsigned i=0; i!=hull.size(); ++i)
  // //      corners[i] = hull[i];
  // // cvFillConvexPoly(img, corners, hull.size(), CV_RGB(255,255,255));

  // cvLine(img, points[0], points[1], colour,lw);
  // cvLine(img, points[1], points[2], colour,lw);
  // cvLine(img, points[2], points[3], colour,lw);
  // cvLine(img, points[3], points[0], colour,lw);

  // cvLine(img, points[4], points[5], colour,lw);
  // cvLine(img, points[5], points[6], colour,lw);
  // cvLine(img, points[6], points[7], colour,lw);
  // cvLine(img, points[7], points[4], colour,lw);

  // cvLine(img, points[8], points[9], colour,lw);
  // cvLine(img, points[9], points[10], colour,lw);
  // cvLine(img, points[10], points[11], colour,lw);
  // cvLine(img, points[11], points[8], colour,lw);

  // cvLine(img, points[0], points[4], colour,lw);
  // cvLine(img, points[1], points[5], colour,lw);
  // cvLine(img, points[2], points[6], colour,lw);
  // cvLine(img, points[3], points[7], colour,lw);

  // cvLine(img, points[4], points[8], colour,lw);
  // cvLine(img, points[5], points[9], colour,lw);
  // cvLine(img, points[6], points[10], colour,lw);
  // cvLine(img, points[7], points[11], colour,lw);

  // // for (unsigned i=1; i<hull.size(); ++i)
  // //      cvLine(img,hull[i-1],hull[i],colour,lw+1);
  // // cvLine(img,hull.back(),hull.front(),colour,lw+1);

  for (unsigned i = 0; i != 7; ++i)
  {
    cvLine(img, points[i], points[i + 1], colour, lw);
    cvLine(img, points[i + 8], points[i + 9], colour, lw);
    cvLine(img, points[i + 16], points[i + 17], colour, lw);
  }
  cvLine(img, points[7], points[0], colour, lw);
  cvLine(img, points[15], points[8], colour, lw);
  cvLine(img, points[23], points[16], colour, lw);
  for (unsigned i = 0; i != 8; ++i)
  {
    cvLine(img, points[i], points[i + 8], colour, lw);
    cvLine(img, points[i + 8], points[i + 16], colour, lw);
  }
}
#endif

void loadCalibrations(const char *filename,const char* intrinsic,const char* extrinsic)
{
  ExtrinsicFile = extrinsic;
  IntrinsicFile = intrinsic;

  XmlReader rd(filename);

  rd.unpack("PersonHeight", personHeight);
  rd.unpack("wg", wg);
  rd.unpack("wm", wm);
  rd.unpack("wt", wt);
  rd.unpack("midRatio", midRatio);
  //     rd.unpack("persDist", persDist);
  rd.unpack("scanres", scanres);
  rd.unpack("w2", w2);

  rd.unpack("Cameras", cam);
  //cam[0].init(); // TODO
}

float loadWorldPriorHull(const char *file, vector<WorldPoint> &polygon)
{
  ifstream ifs(file);
  if (!ifs)
    errx(1, "Cannot open file '%s'", file);

  char buffer[1024];
  ifs.getline(buffer, sizeof(buffer));
  float logInside = atof(buffer);
  ifs.getline(buffer, sizeof(buffer));
  while (!ifs.eof())
  {
    vector<char *> s = splitwhite(buffer, true);
    if (s.size() == 3)
      polygon.push_back(WorldPoint(atoi(s[0]), atoi(s[1]), atoi(s[2])));
    else if (s.size() == 2)
      polygon.push_back(WorldPoint(atoi(s[0]), atoi(s[1]), 0));
    else
    {
      cout << "ERROR " << __PRETTY_FUNCTION__ << " Split size=" << s.size()
          << flush;
      for (unsigned j = 0; j != s.size(); ++j)
        cout << ":" << s[j];
      cout << endl;
    }

    ifs.getline(buffer, sizeof(buffer));

  }
  polygon.push_back(polygon.front());

  return logInside;
}

bool inside(const WorldPoint &p, const vector<WorldPoint> &prior)
{
  unsigned nLeft = 0, nRight = 0;
  for (unsigned i = 1; i < prior.size(); ++i)
  {
    if ((p.y >= prior[i - 1].y && p.y < prior[i].y)
        || (p.y >= prior[i].y && p.y < prior[i - 1].y))
    {
      double deltaX = prior[i].x - prior[i - 1].x, deltaY = prior[i].y
          - prior[i - 1].y;
      if (fabs(deltaX) > fabs(deltaY))
      {
        double a = deltaY / deltaX, b = prior[i].y - a * prior[i].x, x = (p.y
            - b) / a;
        if (x < p.x)
          nLeft++;
        else
          // on the line is out
          nRight++;
      }
      else
      {
        double a = deltaX / deltaY, b = prior[i].x - a * prior[i].y, x = a * p.y
            + b;
        if (x < p.x)
          nLeft++;
        else
          // on the line is out
          nRight++;
      }
    }
  }
  return (nLeft % 2 == 1 && nRight % 2 == 1);
}

/**
 * \brief Given a vector of the same size as scanlocations, create a matrix representation
 * \param probs
 * \return
 *
 * 2012/01/12: GWENN - First version
 *
 **/
struct GridElt
{
    unsigned row, col;
    GridElt(unsigned r, unsigned c) :
        row(r), col(c)
    {
    }
};
vector<GridElt> gridLocations;
unsigned gridWidth, gridHeight;
vector<vector<unsigned> > gridElts;

void gridMatrix(const vnl_vector<FLOAT> &probs, vnl_matrix<FLOAT> &grid)
{
  grid = vnl_matrix<FLOAT>(gridHeight, gridWidth, -1.);
  for (unsigned i = 0; i != probs.size(); ++i)
    grid(gridLocations[i].row, gridLocations[i].col) = probs[i];
}

unsigned shiftedGridElt(unsigned elt, int shiftRow, int shiftCol)
{
  unsigned r = gridLocations[elt].row + shiftRow, c = gridLocations[elt].col
      + shiftCol;
  if (r >= gridHeight || c >= gridWidth)
    return elt;

  if (gridElts[r][c] == (unsigned) -1)
    return elt;
  return gridElts[r][c];
}

void genScanLocations(const vector<WorldPoint> &prior, double scanRes,
    vector<WorldPoint> &sl)
{
  // Find the boundaries of a rectangle in the ground plane
  minX = INFINITY;
  maxX = -INFINITY;
  minY = INFINITY;
  maxY = -INFINITY;

  for (unsigned i = 0; i != prior.size(); ++i)
  {
    if (minX > prior[i].x)
      minX = prior[i].x;
    if (maxX < prior[i].x)
      maxX = prior[i].x;
    if (minY > prior[i].y)
      minY = prior[i].y;
    if (maxY < prior[i].y)
      maxY = prior[i].y;
  }

  sl.clear();
  unsigned r = 0, c = 0;
  gridElts = vector<vector<unsigned> >((maxY - minY) / scanRes + 1,
      vector<unsigned>((maxX - minX) / scanRes + 1, (unsigned) -1));
  // cout << "Grid is " << gridElts.size() << "x" << gridElts[0].size() << endl;
  for (double y = minY; y < maxY; y += scanRes, ++r)
  {
    c = 0;
    for (double x = minX; x < maxX; x += scanRes, ++c)
    {
      WorldPoint pt(x, y);
      if (inside(pt, prior))
      {
        gridLocations.push_back(GridElt(r, c));
        gridElts[r][c] = sl.size();
        sl.push_back(pt);
      }
    }
  }
  // cout << "Grid is " << r << "x" << c << endl;
  gridWidth = c;
  gridHeight = r;
}
