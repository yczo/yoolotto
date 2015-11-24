//
//  RGBA.h
//  TabNav
//
//  Created by David Kammeyer on 10/27/12.
//  Copyright (c) 2012 David Kammeyer. All rights reserved.
//

#ifndef TabNav_RGBA_h
#define TabNav_RGBA_h

#import "Prototype.h"
#import "Image.h"

int RGBA_OCR_Call(unsigned char *ptr, int imgWidth, int imgHeight, char *mainPath,
                  LPTICKETRESULTS lpTicketResults, char *error, char *imgName);

int OCR_Call(unsigned char *ptr, int imgWidth, int imgHeight, char *buffType,
             int byteSteps, int outimages, LPIMGDATA imgOut, char *mainPath,
             LPTICKETRESULTS lpTicketResults, char *error, char *imgName);



#endif
