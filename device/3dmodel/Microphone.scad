EPSILON = 0.001;

MIC_DIAMETER = 9.7;
MIC_HEIGHT = 4.5;

WALL_THICKNESS = 0.8;
MICROPHONE_MARGIN = 0.1;

BASE_THICKNESS_UNDER_MICROPHONE = 0.2;

function Microphone_holderDiameter() = MIC_DIAMETER + 2 * (WALL_THICKNESS + MICROPHONE_MARGIN);

/**
 * Microphone.
 *
 * @param positiveShape
 *     if true, draw only the positive part of the microphone,
 *     if false, draw only the negative part.
 */
module Microphone(positiveShape = true) {
    // Base
    if (positiveShape) {
        translate([0, 0, BASE_THICKNESS_UNDER_MICROPHONE])
            cylinder(h = MIC_HEIGHT, d = MIC_DIAMETER, $fn = 32);
    }
}

/**
 * Microphone holder.
 *
 * @param positiveShape
 *     if true, draw only the positive part of the microphone holder,
 *     if false, draw only the negative part.
 */
module Microphone_holder(baseThickness = 0.8, positiveShape = true) {
    horizontalSupportHeight = 1;
    
    // Base
    baseDiameter = Microphone_holderDiameter();
    if (positiveShape) {
        cylinder(h = baseThickness, d = baseDiameter, $fn = 128);
    }
    
    // Support
    wallheight = BASE_THICKNESS_UNDER_MICROPHONE + MIC_HEIGHT + horizontalSupportHeight;
    if (positiveShape) {
        difference() {
            // Positive part of the walls
            cylinder(h = wallheight, d = baseDiameter, $fn = 128);
            
            // Cut the walls in order to be able to insert the microphone
            width = 3;
            for (x = [3, -3 - width])
                translate([x, -baseDiameter / 2, baseThickness - EPSILON])
                    cube([width, baseDiameter, MIC_HEIGHT + 2]);
        }
    } else {
        // Negative part of the walls
        translate([0, 0, BASE_THICKNESS_UNDER_MICROPHONE - EPSILON])
            cylinder(h = wallheight - horizontalSupportHeight -
                         BASE_THICKNESS_UNDER_MICROPHONE + 2 * EPSILON,
                     d = baseDiameter - 2 * WALL_THICKNESS, $fn = 128);
        
        translate([0, 0, wallheight - horizontalSupportHeight - EPSILON])
            linear_extrude(height = horizontalSupportHeight + 2 * EPSILON,
                           scale=[0.9, 0.9], $fn = 128)
                circle(d = baseDiameter - 2 * WALL_THICKNESS);
    }
    
    // Clearance under the microphone
    if (!positiveShape) {
        width = 6;
        depth = 1;
        height = BASE_THICKNESS_UNDER_MICROPHONE + 2 * EPSILON;
        
        for (ypos = [-1.5, 1.5]) {
            translate([
                - width / 2,
                - depth / 2 + ypos,
                -EPSILON
            ]) {
                translate([0, depth / 2, 0])
                    cylinder(h = height, r = depth / 2, $fn = 32);
                translate([width, depth / 2, 0])
                    cylinder(h = height, r = depth / 2, $fn = 32);
                cube([width, depth, height]);
            }
        }
    }
}

/*
color("red")
difference() {
    Microphone(true);
    Microphone(false);
}
*/
difference() {
    Microphone_holder(0.8, true);
    Microphone_holder(0.8, false);
}