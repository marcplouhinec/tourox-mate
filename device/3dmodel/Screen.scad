SCREEN_WIDTH = 30;
SCREEN_DEPTH = 1.54 * 25.4;
SCREEN_HEIGHT = 1.7;

SCREEN_HOLE_DIAMETER = 0.12 * 25.4;
SCREEN_HOLE_MARGIN = 1.7;
SCREEN_HORIZONTAL_SUPPORT_WIDTH = 1.34 * 25.4 + SCREEN_HOLE_DIAMETER + 2 * SCREEN_HOLE_MARGIN;
SCREEN_HORIZONTAL_SUPPORT_DEPTH = SCREEN_HOLE_DIAMETER + 2 * SCREEN_HOLE_MARGIN;

SCREEN_SUPPORT_MARGIN = 0;

SCREEN_GLASS_WIDTH = SCREEN_WIDTH - 1;
SCREEN_GLASS_HEIGHT = 1.5;
SCREEN_GLASS_DEPTH = 32.5;
SCREEN_GLASS_MARGIN_TOP = 5.5;

function Screen_totalWidth() = SCREEN_HORIZONTAL_SUPPORT_WIDTH;
function Screen_xPosition() =
    (SCREEN_HORIZONTAL_SUPPORT_WIDTH - SCREEN_WIDTH) / 2 + SCREEN_SUPPORT_MARGIN;

function Screen_holderWidth() = SCREEN_HORIZONTAL_SUPPORT_WIDTH + SCREEN_SUPPORT_MARGIN;

/**
 * Screen.
 *
 * @param positiveShape
 *     if true, draw only the positive part of the screen,
 *     if false, draw only the negative part.
 */
module Screen(positiveShape = true) {
    translate([(SCREEN_HORIZONTAL_SUPPORT_WIDTH - SCREEN_WIDTH) / 2 + SCREEN_SUPPORT_MARGIN, 0, 0]) {
        // Base
        if (positiveShape) {
            translate([0, -SCREEN_DEPTH, SCREEN_GLASS_HEIGHT])
                cube([SCREEN_WIDTH, SCREEN_DEPTH, SCREEN_HEIGHT]);
        }
        
        // Horizontal supports
        if (positiveShape) {
            width = SCREEN_HORIZONTAL_SUPPORT_WIDTH;
            depth = SCREEN_HORIZONTAL_SUPPORT_DEPTH;
            cornerRadius = 3.2;
            
            for (y = [0, -SCREEN_DEPTH + depth])
                translate([(SCREEN_WIDTH - width) / 2, -depth + y, SCREEN_GLASS_HEIGHT])
                    difference() {
                        linear_extrude(height = SCREEN_HEIGHT)
                            offset(r = cornerRadius, $fn = 32)
                                offset(r = -cornerRadius, $fn = 32)
                                    square([width, depth]);
                        
                        for (x = [SCREEN_HOLE_DIAMETER, width - SCREEN_HOLE_DIAMETER])
                            translate([x, SCREEN_HOLE_DIAMETER, -0.5])
                                cylinder(d = SCREEN_HOLE_DIAMETER, h = SCREEN_HEIGHT * 2, $fn = 32);
                    }
        }
        
        // Glass
        if (positiveShape) {
            marginTop = SCREEN_GLASS_MARGIN_TOP;
            
            translate([(SCREEN_WIDTH - SCREEN_GLASS_WIDTH)/2, -SCREEN_GLASS_DEPTH - marginTop, 0])
                cube([SCREEN_GLASS_WIDTH, SCREEN_GLASS_DEPTH, SCREEN_GLASS_HEIGHT]);
        }
    }
}

/**
 * Screen holder.
 *
 * @param positiveShape
 *     if true, draw only the positive part of the screen holder,
 *     if false, draw only the negative part.
 */
module Screen_holder(coverHeight = 1, positiveShape = true) {
    EPSILON = 0.001;
    coverThicknessUnderScreen = 0.2;
    
    // Base
    if (positiveShape) {
        cornerRadius = 4;
        translate([0, -SCREEN_DEPTH - SCREEN_SUPPORT_MARGIN, 0])
            linear_extrude(height = coverHeight)
                offset(r = cornerRadius, $fn = 32)
                    offset(r = -cornerRadius, $fn = 32)
                        square([
                            SCREEN_HORIZONTAL_SUPPORT_WIDTH + 2 * SCREEN_SUPPORT_MARGIN,
                            SCREEN_DEPTH + 2 * SCREEN_SUPPORT_MARGIN
                        ]);
    }
    
    translate([(SCREEN_HORIZONTAL_SUPPORT_WIDTH - SCREEN_WIDTH) / 2 + SCREEN_SUPPORT_MARGIN, 0, 0]) {
        
        // Glass recess
        if (!positiveShape) {
            width = SCREEN_GLASS_WIDTH;
            depth = SCREEN_GLASS_DEPTH;
            cornerRadius = 0.6;
            
            translate([(SCREEN_WIDTH - width) / 2,
                       -depth - SCREEN_GLASS_MARGIN_TOP,
                       coverThicknessUnderScreen])
                linear_extrude(height = SCREEN_GLASS_HEIGHT)
                    offset(r = cornerRadius, $fn = 32)
                        square([width, depth]);
        }
        
        // Clearance
        if (!positiveShape) {
            width = 25.6;
            depth = 24.5;
            cornerRadius = 1;
            
            translate([(SCREEN_WIDTH - width) / 2, -depth - 8, -coverHeight - EPSILON])
                linear_extrude(height = coverHeight * 2)
                    offset(r = cornerRadius, $fn = 32)
                        offset(r = -cornerRadius, $fn = 32)
                            square([width, depth]);
        }
        
        // Cylinders
        if (positiveShape) {
            diameter = SCREEN_HOLE_DIAMETER - 0.6;
            height = SCREEN_GLASS_HEIGHT + SCREEN_HEIGHT;
            supportHeight = SCREEN_GLASS_HEIGHT - 0.1;
            
            for (x = [-2.2, 32.2])
                for (y = [-3.4, -36.1])
                    translate([x, y, 0]) {
                        cylinder(d = diameter, h = height, $fn = 32);
                        cylinder(d = diameter * 1.6, h = supportHeight, $fn = 32);
                    }
        }
        
        // Vertical supports
        if (positiveShape) {
            depth = SCREEN_DEPTH - 2 * SCREEN_HORIZONTAL_SUPPORT_DEPTH - 12;
            width = 2;
            wallWidth = 0.001;
            wallHeight = width + SCREEN_GLASS_HEIGHT + SCREEN_HEIGHT - 0.2 + coverThicknessUnderScreen;
            roundingRadius = 0.6;
            
            xpositions = [-roundingRadius - 0.4, roundingRadius + 0.4 + SCREEN_WIDTH];
            
            for (x = xpositions)
                translate([
                    x,
                    -depth - (SCREEN_DEPTH - depth) / 2 + (x == xpositions[0] ? 0 : depth),
                    wallHeight
                ]) {
                    rotate([0, 0, x == xpositions[0] ? 0 : -180])
                        difference() {
                            minkowski() {
                                union() {
                                    rotate([-90, 0, 0])
                                        linear_extrude(height = depth)
                                            polygon(points = [[0, 0], [width / 1.5, 0], [0, width]]);
                                    translate([0, depth, 0])
                                        rotate([90, 0, 0])
                                            linear_extrude(height = depth)
                                                polygon(points = [
                                                    [0, 0], [width / 1.5, 0], [0, width]
                                                ]);
                                    
                                    translate([-wallWidth, 0, -wallHeight])
                                        cube([wallWidth, depth, wallHeight + width]);
                                }
                                cylinder(r = roundingRadius, h = 0.1, $fn = 32);
                            }
                            
                            translate([-2, -roundingRadius - EPSILON, 0])
                                cube([5, depth + 2 * roundingRadius + 2 * EPSILON, 5]);
                        }
                }
        }
    }
}
/*
color("blue")
difference() {
    Screen(true);
    Screen(false);
}
*/
difference() {
    Screen_holder(0.8, true);
    Screen_holder(0.8, false);
}
