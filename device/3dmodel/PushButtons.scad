EPSILON = 0.001;

BUTTON_SIZE = 6;
BUTTON_HEIGHT = 4;

WALL_THICKNESS = 1.2;
BUTTON_MARGIN = 0.1;

/**
 * Buttons.
 *
 * @param positiveShape
 *     if true, draw only the positive part.
 *     if false, draw only the negative part.
 */
module PushButtons(baseThickness = 0.8,
                   distanceBetweenButtons = 30,
                   positiveShape = true) {
    if (positiveShape) {
        for (x = [-distanceBetweenButtons / 2, distanceBetweenButtons / 2])
            translate([x, 0, baseThickness]) {
                // Base
                translate([-BUTTON_SIZE / 2, -BUTTON_SIZE / 2, 0])
                    cube([BUTTON_SIZE, BUTTON_SIZE, BUTTON_HEIGHT]);
                
                // Moving part
                diameter = 3.5;
                
                translate([diameter / 2 + (BUTTON_SIZE - diameter) / 2 - BUTTON_SIZE / 2,
                           diameter / 2 + (BUTTON_SIZE - diameter) / 2 - BUTTON_SIZE / 2,
                           0])
                    cylinder(h = 5.1, d = diameter, $fn = 32);
            }
    }
}


/**
 * Button holders.
 *
 * @param positiveShape
 *     if true, draw only the positive part.
 *     if false, draw only the negative part.
 */
module PushButtons_holder(baseThickness = 0.8, distanceBetweenButtons = 30, positiveShape = true) {
    for (x = [-distanceBetweenButtons / 2, distanceBetweenButtons / 2]) {
        // Base
        baseSize = BUTTON_SIZE + 2 * (WALL_THICKNESS + BUTTON_MARGIN);
        if (positiveShape) {
            translate([-baseSize / 2 + x, -baseSize / 2, 0])
                linear_extrude(height = baseThickness)
                    offset(r = 3, $fn = 64)
                        offset(r = -3, $fn = 64)
                            square([baseSize, baseSize]);
        }
        
        // Horizontal wall around each button
        wallWidth = BUTTON_SIZE - 3;
        wallHeight = BUTTON_HEIGHT + baseThickness;
        if (positiveShape) {
            for (y = [-BUTTON_SIZE / 2 - WALL_THICKNESS / 2 - BUTTON_MARGIN,
                      BUTTON_SIZE / 2 + WALL_THICKNESS / 2 + BUTTON_MARGIN])
                translate([-wallWidth / 2 + x, -WALL_THICKNESS / 2 + y, 0]) {
                    cube([wallWidth, WALL_THICKNESS, wallHeight]);
                }
        }
        
        // Vertical wall around each button
        if (positiveShape) {
            for (x = [-(distanceBetweenButtons + BUTTON_SIZE + WALL_THICKNESS) / 2 - BUTTON_MARGIN,
                      -(distanceBetweenButtons - BUTTON_SIZE - WALL_THICKNESS) / 2 + BUTTON_MARGIN,
                      (distanceBetweenButtons - BUTTON_SIZE - WALL_THICKNESS) / 2 - BUTTON_MARGIN,
                      (distanceBetweenButtons + BUTTON_SIZE + WALL_THICKNESS) / 2 + BUTTON_MARGIN]) {
                translate([-WALL_THICKNESS / 2 + x, -wallWidth / 2, 0]) {
                    cube([WALL_THICKNESS, wallWidth, wallHeight]);
                }
            }
        }
    }
}

/**
 * Buttons cover on the front panel.
 *
 * @param positiveShape
 *     if true, draw only the positive part.
 *     if false, draw only the negative part.
 */
module PushButtons_cover(baseThickness = 0.8, width = 50, depth = 18, positiveShape = true) {
    // Cut the buttons in the cover
    if (!positiveShape) {
        gap = 0.4;
        centralSupportWidth = 10;
        
        difference() {
            translate([0, 0, -EPSILON])
                linear_extrude(height = baseThickness + 2 * EPSILON)
                    difference() {
                        _PushButtons_coverShape(width, depth);
                        translate([gap, gap])
                            _PushButtons_coverShape(width - 2 * gap, depth - 2 * gap);
                    }
                
            // Central holder
            translate([(width - centralSupportWidth) / 2, 0, -0.1])
                cube([centralSupportWidth, depth * 2, baseThickness]);
        }
    }
}

module _PushButtons_coverShape(width = 50, depth = 18) {
    circleRadius = width / 2;
    cornerRadius = 4;
    outerCornerRadius = 2;
    
    offset(r = outerCornerRadius, $fn = 32)
        offset(r = -outerCornerRadius, $fn = 32)
            difference() {
                offset(r = cornerRadius, $fn = 32)
                    offset(r = -cornerRadius, $fn = 32) {
                            difference() {
                                translate([circleRadius, circleRadius])
                                    intersection() {
                                        scale([1.5, 1])
                                            circle(r = circleRadius, $fn = 128);
                                        
                                        trapezoidRatio = 1/30;
                                        polygon(points = [
                                            [-width / 2, depth / 2],
                                            [-width / 2 + trapezoidRatio * width, -width / 2],
                                            [width / 2 - trapezoidRatio * width, -width / 2],
                                            [width / 2, depth / 2]
                                        ]);
                                    }
                                
                                translate([width / 2, -1])
                                    offset(r = cornerRadius, $fn = 32)
                                        polygon(points = [
                                                [-4, 3],
                                                [4, 3],
                                                [6.6, 0],
                                                [-6.6, 0]
                                        ]);
                            }
                        }

                translate([circleRadius, circleRadius + depth])
                    scale([2, 1])
                        circle(r = circleRadius, $fn = 128);
            }
}



color("gray")
    difference() {
        PushButtons(0.8, 30, true);
        PushButtons(0.8, 30, false);
    }
    
difference() {
    PushButtons_holder(0.8, 30, true);
    PushButtons_holder(0.8, 30, false);
}

/*
difference() {
    PushButtons_cover(baseThickness = 0.8, positiveShape = true);
    PushButtons_cover(baseThickness = 0.8, positiveShape = false);
}
*/