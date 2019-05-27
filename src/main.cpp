#include "FrameBuffer.h"
#include "FbProperties.h"
#include "FbErrno.h"

#include <unistd.h>
#include <cstdio>
#include <cmath>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

int main (int argc, char **argv)
{
    using namespace sora;
    using namespace cv;

    BasicFbdrv drv;
    /*
    Mat frame = imread (argv[1]);
    if (frame.empty ()) {
        std::cerr << "Cannot open file!\n";

        return 1;
    }
    */

    VideoCapture capture (argv[1]);

    Vec3b pixel;
    Vec3b *line;
    Mat frame;

    while (true) {
        capture >> frame;

        for (int i = 0; i < frame.rows; i++) {
            line = frame.ptr<Vec3b> (i);

            for (int j = 0; j < frame.cols; j++) {
                pixel = line[j];
            
                drv.set_RGB_option (pixel[2], pixel[1], pixel[0]);
                drv.set_transp_option (0);
                try {
                    drv.write_graphics_mem (j, i);
                } catch (int e) {
                    continue;
                }

                // usleep (200);
                // drv.flush_screen ();
            }
            // std::cout << std::endl;
        }
    }

    getchar ();
    drv.flush_screen ();

    /*
    int y, a, b;

    drv.set_RGB_option (255, 0, 0);
    drv.set_transp_option (0);
	for (a = 960, b = 960; a != 30 && b != 30; a -= 10, b -= 10)
	{
		for (int x = (b - a); x != (a + b); x++) {
			y = (int) sqrt (a * a - (x - b) * (x - b));
			
            drv.write_fbdev_address (x, y);
			usleep (200);
		}
	}
    */

    return 0;
}