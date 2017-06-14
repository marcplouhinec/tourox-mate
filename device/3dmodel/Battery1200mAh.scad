EPSILON = 0.001;

BATTERY1200_WIDTH = 34;
BATTERY1200_DEPTH = 60;
BATTERY1200_HEIGHT = 5;

BATTERY1200_HOLDER_WALL_THICKNESS = 0.8;
BATTERY1200_MARGIN = 0.2;

function Battery1200mAh_holderWidth() =
    BATTERY1200_WIDTH + 2 * (BATTERY1200_HOLDER_WALL_THICKNESS + BATTERY1200_MARGIN);
function Battery1200mAh_holderDepth() =
    BATTERY1200_DEPTH + 2 * (BATTERY1200_HOLDER_WALL_THICKNESS + BATTERY1200_MARGIN);

/**
 * 1200 mAh Battery.
 *
 * @param positiveShape
 *     if true, draw only the positive part.
 *     if false, draw only the negative part.
 */
module Battery1200mAh(baseThickness = 0.8, positiveShape = true) {
    if (positiveShape) {
        translate([BATTERY1200_HOLDER_WALL_THICKNESS + BATTERY1200_MARGIN,
                   BATTERY1200_HOLDER_WALL_THICKNESS + BATTERY1200_MARGIN,
                   baseThickness])
            cube([BATTERY1200_WIDTH, BATTERY1200_DEPTH, BATTERY1200_HEIGHT]);
    }
}

/**
 * 1200 mAh Battery holder.
 *
 * @param positiveShape
 *     if true, draw only the positive part.
 *     if false, draw only the negative part.
 */
module Battery1200mAh_holder(baseThickness = 0.8, positiveShape = true) {
    // Base
    baseWidth = Battery1200mAh_holderWidth();
    baseHeight = Battery1200mAh_holderDepth();
    if (positiveShape)
        cube([baseWidth, baseHeight, baseThickness]);
    
    // Top and bottom walls
    if (positiveShape) {
        wallWidth = 8;
        wallSideMargin = 4;
        for (x = [wallSideMargin, baseWidth - wallWidth - wallSideMargin])
            for (y = [0, baseHeight - baseThickness + BATTERY1200_MARGIN])
                translate([x, y, 0])
                    cube([wallWidth,
                          BATTERY1200_HOLDER_WALL_THICKNESS,
                          BATTERY1200_HEIGHT + baseThickness]);
    }
    
    // Left and right walls
    if (positiveShape) {
        wallDepth = 40;
        horizontalHolderWidth = BATTERY1200_HOLDER_WALL_THICKNESS;
        horizontalHolderHeight = BATTERY1200_HOLDER_WALL_THICKNESS + baseThickness + 
                                 BATTERY1200_HEIGHT - 1;
        
        for (x = [0, baseWidth])
            translate([x, (baseHeight - wallDepth) / 2 + (x > 0 ? wallDepth : 0), 0])
                rotate([0, 0, x > 0 ? 180 : 0]) {
                    cube([BATTERY1200_HOLDER_WALL_THICKNESS, wallDepth, horizontalHolderHeight]);
                    
                    translate([horizontalHolderWidth - EPSILON, 0, horizontalHolderHeight])
                        rotate([-90, 0, 0])
                            linear_extrude(height = wallDepth)
                                polygon(points = [[0, 0],
                                                 [horizontalHolderWidth, 0],
                                                 [0, horizontalHolderWidth + 0.2]]);
                }
            
    }
}

color("orange")
    difference() {
        Battery1200mAh(1, true);
        Battery1200mAh(1, false);
    }

difference() {
    Battery1200mAh_holder(1, true);
    Battery1200mAh_holder(1, false);
}