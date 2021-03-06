function L = get_FOV_masks( session,image, description, zct )

% Copyright (C) 2013 Imperial College London.
% All rights reserved.
%
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2 of the License, or
% (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License along
% with this program; if not, write to the Free Software Foundation, Inc.,
% 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
%
% This software tool was developed with support from the UK 
% Engineering and Physical Sciences Council 
% through  a studentship from the Institute of Chemical Biology 
% and The Wellcome Trust through a grant entitled 
% "The Open Microscopy Environment: Image Informatics for Biological Sciences" (Ref: 095931).

pixelsList = image.copyPixels();    
pixels = pixelsList.get(0);
            
imH = pixels.getSizeX().getValue();
imW = pixels.getSizeY().getValue();

segmmask_restored = zeros(imW,imH);

service = session.getRoiService();
roiResult = service.findByImage(image.getId.getValue, []);
rois = roiResult.rois;
n = rois.size;
for thisROI  = 1:n
    roi = rois.get(thisROI-1);
    
    if isempty(description) || strcmp(char(roi.getDescription().getValue()),description)
        
        numShapes = roi.sizeOfShapes; % an ROI can have multiple shapes.
        for ns = 1:numShapes
            shape = roi.getShape(ns-1); % the shape 
           
             if (isa(shape, 'omero.model.Mask'))
                 
                thiszct = [ shape.getTheZ.getValue() shape.getTheC.getValue() shape.getTheT.getValue()];
                
                if thiszct == zct
                 
                    x0 = shape.getX().getValue;
                    y0 = shape.getY().getValue;
                    W = shape.getWidth().getValue;
                    H = shape.getHeight().getValue;
                    % retrieve the mask data
                    bytes = shape.getBytes();

                    % code to convert byte mask to double
                    dvec = zeros(1,length(bytes) .* 8);
                    outputctr = 1;
                    for b = 1:length(bytes)
                        dvec(outputctr:outputctr + 7) = bitget(bytes(b),8:-1:1);
                        outputctr = outputctr + 8;
                    end
                    %truncate double vector to size of original mask
                    dvec = dvec(1:H.*W);

                    mask = reshape(dvec,W,H);

                    for x=1:W
                        for y=1:H
                            if(0~=mask(x,y))
                                segmmask_restored(y0+y,x0+x) = 1;
                            end                    
                        end
                    end
                end

             end
        end
        
    end
end

L = bwlabel(segmmask_restored);
L = L(1:imW,1:imH); % ??

end

