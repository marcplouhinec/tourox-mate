EPSILON = 0.001;

WALL_THICKNESS = 0.4;

module Lanyard_holder(baseThickness = 0.8, positiveShape = true) {
    height = 4;
    clearanceCircleScaleX = 1/2;
    cylinderDiameter = 2;
    
    // Wrapping base
    if (positiveShape) {
        difference() {
            height = height + baseThickness + WALL_THICKNESS;
            diameter = 2 * height * clearanceCircleScaleX + cylinderDiameter + 2 * WALL_THICKNESS;
            scale([2, 1]) {
                cylinder(h = height,
                         d = diameter,
                         $fn = 128);
                translate([-diameter / 2, -diameter / 2 - cylinderDiameter / 2, -EPSILON])
                    cube([diameter, diameter / 2, height + 2 * EPSILON]);
            }
        }
    }
    
    // Holder cylinder
    if (positiveShape) {
        cylinder(h = height + baseThickness + 2 * EPSILON, d = cylinderDiameter, $fn=32);
    }
    
    // Clearance around the cylinder
    if (!positiveShape) {
        translate([0, 0, baseThickness + height / 2]) {
            rotate_extrude(convexity = 10, $fn=32)
                translate([(height * clearanceCircleScaleX) / 2 + cylinderDiameter / 2, 0])
                    scale([clearanceCircleScaleX, 1])
                        circle(d = height, $fn = 32);
        }
        translate([0, 0, baseThickness])
            difference() {
                cylinder(d = height * clearanceCircleScaleX + cylinderDiameter,
                          h = height, $fn = 32);
                translate([0, 0, -EPSILON])
                    cylinder(d = cylinderDiameter, h = height + 2 * EPSILON, $fn = 32);
            }
    }
    
    // Clearance on the side of the cylinder
    if (!positiveShape) {
        for (angle = [-70, 70])
            translate([0,
                       (cylinderDiameter + height * clearanceCircleScaleX) / 2,
                       baseThickness + height / 2])
                rotate([90, 0, angle])
                    scale([clearanceCircleScaleX, 1]) {
                        cylinder(d = height, h = 10, $fn=32);
                        translate([angle > 0 ? -height * 2 : 0, -height / 2, height / 2])
                            cube([height * 2, height, 10]);
                    }
    }
}

difference() {
    Lanyard_holder(0.8, true);
    Lanyard_holder(0.8, false);
}