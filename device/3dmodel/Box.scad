use <FrontCoverDecoration.scad>;
use <BackCoverDecoration.scad>;

_BOX_OUTER_COVER_WIDTH = 60.8;
_BOX_OUTER_COVER_DEPTH = 121.4;

_BOX_BACK_OUTER_COVER_HEIGHT = 7.5;
_BOX_FRONT_OUTER_COVER_HEIGHT = 7.5;

_BOX_COVER_THICKNESS = 0.8;

_BOX_BACK_COVER_INNER_CONTOUR_HEIGHT = 2;
_BOX_BACK_COVER_INNER_CONTOUR_THICKNESS = 0.8;
_BOX_BACK_COVER_INNER_CONTOUR_GAP = 0.2;

_FRONT_COVER_FASTENER_DEPTH = 10;

EPSILON = 0.001;


function Box_outerCoverWidth() = _BOX_OUTER_COVER_WIDTH;
function Box_outerCoverDepth() = _BOX_OUTER_COVER_DEPTH;
function Box_coverThickness() = _BOX_COVER_THICKNESS;

function Box_backOuterCoverHeight() = _BOX_BACK_OUTER_COVER_HEIGHT;

/**
 * Shape of the outer cover.
 */
module _Box_outerCoverShape(width = _BOX_OUTER_COVER_WIDTH, depth = _BOX_OUTER_COVER_DEPTH) {
    cornerRadius = 4;
    topSemiCircleDepth = width / 2;
    bottomSemiCircleDepth = width / 2;
    
    offset(r = cornerRadius, $fn = 32)
        offset(r = -cornerRadius, $fn = 32) {
            translate([0, bottomSemiCircleDepth])
                square([width, depth - bottomSemiCircleDepth - topSemiCircleDepth]);
            
            translate([width / 2, depth - bottomSemiCircleDepth])
                intersection() {
                    scale([2, 1])
                        circle(r = topSemiCircleDepth, $fn = 128);
                    square([width, width], center = true);
                }
            
            translate([width / 2, bottomSemiCircleDepth])
                intersection() {
                    scale([1.5, 1])
                        circle(d = width, $fn = 128);
                    square([width, width], center = true);
                }
        }
}

/**
 * Extruded cover with chamfer.
 *
 * @param fill if true, fill the cover with material.
 */
module Box_cover(thickness = _BOX_COVER_THICKNESS, height = 8, chamferAngle = 50, fill = false, padding = 0) {
    diff = height * 3 / 10;
    
    totalWidth = (_BOX_OUTER_COVER_WIDTH) + padding;
    totalDepth = (_BOX_OUTER_COVER_DEPTH) + padding;
    
    baseWidth = totalWidth - diff;
    baseDepth = totalDepth - diff;
    chamferHeight = diff * tan(chamferAngle);
    
    // Base
    translate([diff / 2, diff / 2, 0])
        linear_extrude(height = thickness)
            _Box_outerCoverShape(width = baseWidth, depth = baseDepth);
    
    // Chamfer
    scale = [totalWidth / baseWidth, totalDepth / baseDepth];
    translate([baseWidth / 2 + diff / 2, baseDepth / 2 + diff / 2, 0])
        difference() {
            linear_extrude(height = chamferHeight, scale = scale)
                translate([-baseWidth / 2, -baseDepth / 2, 0])
                    _Box_outerCoverShape(width = baseWidth, depth = baseDepth);
            
            if (!fill)
                translate([thickness, thickness, -EPSILON])
                    linear_extrude(height = chamferHeight + 2 * EPSILON, scale = scale)
                        translate([-baseWidth / 2, -baseDepth / 2, 0])
                            _Box_outerCoverShape(
                                width = baseWidth - 2 * thickness,
                                depth = baseDepth - 2 * thickness);
        }
    
    // Walls
    translate([0, 0, chamferHeight - EPSILON])
        difference() {
            linear_extrude(height = height - chamferHeight)
                _Box_outerCoverShape(width = totalWidth, depth = totalDepth);
            
            if (!fill)
                translate([thickness, thickness, -EPSILON])
                    linear_extrude(height = height - chamferHeight + 2 * EPSILON)
                        _Box_outerCoverShape(
                            width = totalWidth - 2 * thickness,
                            depth = totalDepth - 2 * thickness);
        }
}

/**
 * Front cover.
 */
module Box_frontCover() {
    Box_cover(height = _BOX_FRONT_OUTER_COVER_HEIGHT);
    
    // Fasteners with the back cover
    _Box_allFrontCoverFasteners();
}

/**
 * Back cover.
 */
module Box_backCover() {
    Box_cover(height = _BOX_BACK_OUTER_COVER_HEIGHT);
    
    // Inner contour
    innerContourThickness = _BOX_BACK_COVER_INNER_CONTOUR_THICKNESS;
    innerContourGap = _BOX_BACK_COVER_INNER_CONTOUR_GAP;
    innerContourDimension = [
        _BOX_OUTER_COVER_WIDTH - 2 * _BOX_COVER_THICKNESS - 2 * innerContourGap,
        _BOX_OUTER_COVER_DEPTH - 2 * _BOX_COVER_THICKNESS - 2 * innerContourGap
    ];
    innerContourClearanceDimension = [
        innerContourDimension[0] - 2 * innerContourThickness,
        innerContourDimension[1] - 2 * innerContourThickness
    ];
    
    translate([_BOX_COVER_THICKNESS + innerContourGap, _BOX_COVER_THICKNESS + innerContourGap, _BOX_BACK_OUTER_COVER_HEIGHT]) {
        // Outside part
        linear_extrude(height = _BOX_BACK_COVER_INNER_CONTOUR_HEIGHT)
            difference() {
                _Box_outerCoverShape(width = innerContourDimension[0], depth = innerContourDimension[1]);
                
                translate([innerContourThickness, innerContourThickness, 0])
                    _Box_outerCoverShape(width = innerContourClearanceDimension[0], depth = innerContourClearanceDimension[1]);
            }
            
        // Inside part
        insidePartDimension = [
            innerContourDimension[0] + 2 * innerContourGap + 2 * EPSILON,
            innerContourDimension[1] + 2 * innerContourGap + 2 * EPSILON
        ];
        translate([-EPSILON - innerContourGap, -EPSILON - innerContourGap, -_BOX_BACK_COVER_INNER_CONTOUR_HEIGHT + EPSILON])
            difference() {
                linear_extrude(height = _BOX_BACK_COVER_INNER_CONTOUR_HEIGHT)
                    _Box_outerCoverShape(width = insidePartDimension[0], depth = insidePartDimension[1]);
                
                translate([insidePartDimension[0] / 2, insidePartDimension[1] / 2, -EPSILON])
                    linear_extrude(
                        height = _BOX_BACK_COVER_INNER_CONTOUR_HEIGHT + 2 * EPSILON,
                        scale = [
                            innerContourClearanceDimension[0] / insidePartDimension[0],
                            innerContourClearanceDimension[1] / insidePartDimension[1]
                        ]
                    ) {
                        translate([-insidePartDimension[0] / 2, -insidePartDimension[1] / 2, 0])
                            _Box_outerCoverShape(width = insidePartDimension[0], depth = insidePartDimension[1]);
                    }
            }
    }

}

module _Box_singleFrontCoverFastener() {
    fastenerWidth = 1.2;
    fastenerDepth = _FRONT_COVER_FASTENER_DEPTH;
    fastenerHeight = _BOX_BACK_OUTER_COVER_HEIGHT - _BOX_COVER_THICKNESS;
    interFastenerGap = 0;
    fastenerOtherCoverGap = 0.2;
    holeHeight = 3.25;
    holeZPos = fastenerHeight - _BOX_BACK_COVER_INNER_CONTOUR_HEIGHT + _BOX_BACK_COVER_INNER_CONTOUR_HEIGHT;
    distanceBetweenFrontCoverBaseAndBackCoverContour = fastenerHeight - fastenerOtherCoverGap - _BOX_BACK_COVER_INNER_CONTOUR_HEIGHT;
    
    // Part that goes inside the back cover
    startZPos = 0;
    translate([interFastenerGap + _BOX_BACK_COVER_INNER_CONTOUR_THICKNESS + _BOX_BACK_COVER_INNER_CONTOUR_GAP, 0, startZPos])
        cube([fastenerWidth, fastenerDepth, fastenerHeight - fastenerOtherCoverGap + holeZPos - holeHeight - startZPos]);
    
    // Link the part that goes inside the back cover to the wall of the front cover
    translate([-EPSILON, 0, 0])
        difference() {
            width = _BOX_BACK_COVER_INNER_CONTOUR_THICKNESS + _BOX_BACK_COVER_INNER_CONTOUR_GAP + interFastenerGap + 2 * EPSILON;
            cube([
                _BOX_BACK_COVER_INNER_CONTOUR_THICKNESS + _BOX_BACK_COVER_INNER_CONTOUR_GAP + interFastenerGap + 2 * EPSILON,
                fastenerDepth,
                distanceBetweenFrontCoverBaseAndBackCoverContour
            ]);
        }
    
    // Fill the hole of the back cover fastener part
    extraWidthForStrongFastening = 0.1;
    difference() {
        holeWidth = _BOX_BACK_COVER_INNER_CONTOUR_THICKNESS + _BOX_BACK_COVER_INNER_CONTOUR_GAP + extraWidthForStrongFastening;
        translate([interFastenerGap - extraWidthForStrongFastening, 0, holeZPos])
            cube([holeWidth, fastenerDepth, holeHeight]);
        
        // Smooth the hole filler bottom
        translate([interFastenerGap - EPSILON - extraWidthForStrongFastening, -EPSILON, holeZPos - EPSILON]) {
            rotate([-90, -90, 0])
                linear_extrude(height = fastenerDepth + 2 * EPSILON)
                    polygon(points=[[0,0],[holeWidth + 1,0],[0,holeWidth]]);
        }
    }
}

/**
 * Fastener for maintaining the front cover with the back cover.
 *
 * @param leftSide If true, create the fasteners for the left side. If false, create the fasteners for the right side.
 */
module _Box_frontCoverFastener(leftSide = true) {
    translation = leftSide ? 
        [_BOX_COVER_THICKNESS, 0, _BOX_COVER_THICKNESS] :
        [-_BOX_COVER_THICKNESS + _BOX_OUTER_COVER_WIDTH, _FRONT_COVER_FASTENER_DEPTH, _BOX_COVER_THICKNESS];
    
    translate(translation)
        rotate([0, 0, leftSide ? 0 : 180])
            _Box_singleFrontCoverFastener();
}

/**
 * All the fastener positioned.
 */
module _Box_allFrontCoverFasteners() {
    yPositions = [12, _BOX_OUTER_COVER_DEPTH / 2, _BOX_OUTER_COVER_DEPTH - 19];
    for (y = yPositions)
        translate([0, y, 0]) {
            _Box_frontCoverFastener(leftSide = true);
            
            if (y != yPositions[2]) // Do not create a fastener that would block the FONA
                _Box_frontCoverFastener(leftSide = false);
        }
        
    // Replace the missing fastener next to the FONA by a horizontal one
    translate([_BOX_OUTER_COVER_WIDTH / 2 - _FRONT_COVER_FASTENER_DEPTH / 2, _BOX_OUTER_COVER_DEPTH - 1.4, 0])
        rotate([0, 0, -90])
            _Box_singleFrontCoverFastener();
}

module Box_assembled(includeBackCover = true, includeFrontCover = true, includeComponents = true) {
    difference() {
        union() {
            translate([
                _BOX_OUTER_COVER_WIDTH,
                0,
                _BOX_FRONT_OUTER_COVER_HEIGHT + _BOX_BACK_OUTER_COVER_HEIGHT
            ]) {
                rotate([0, 180, 0]) {
                    if (includeFrontCover) {
                        Box_frontCover();
                        FrontCoverDecoration(positiveShape = true);
                    }
                }
            }
            if (includeBackCover) {
                Box_backCover();
                BackCoverDecoration(positiveShape = true, includeComponents = includeComponents);
            }
        }
        
        union() {
            translate([
                _BOX_OUTER_COVER_WIDTH,
                0,
                _BOX_FRONT_OUTER_COVER_HEIGHT + _BOX_BACK_OUTER_COVER_HEIGHT
            ]) {
                rotate([0, 180, 0]) {
                    FrontCoverDecoration(positiveShape = false);
                }
            }
            BackCoverDecoration(positiveShape = false, includeComponents = includeComponents);
        }
    }
}

//rotate([0, 180, 0])
Box_assembled(includeBackCover = true, includeFrontCover = true, includeComponents = false);