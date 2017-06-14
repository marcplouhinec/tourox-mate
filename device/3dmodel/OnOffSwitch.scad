EPSILON = 0.001;

BODY_WIDTH = 6.4;
BODY_DEPTH = 11.6;
BODY_HEIGHT = 4;

MOVING_PART_SIZE = 2;
PINS_WIDTH = 4.6;
PINS_DEPTH = 5;
PINS_HEIGHT = 0.5;

HOLDER_WALL_THICKNESS = 2;
BODY_MARGIN = 0.1;

function OnOffSwitch_width() = BODY_WIDTH;
function OnOffSwitch_depth() = BODY_DEPTH;

function OnOffSwitch_holderWidth() = BODY_WIDTH + HOLDER_WALL_THICKNESS + BODY_MARGIN;
function OnOffSwitch_holderDepth() = BODY_DEPTH + 2 * (HOLDER_WALL_THICKNESS + BODY_MARGIN);

/**
 * ON-OFF switch.
 *
 * @param positiveShape
 *     if true, draw only the positive part.
 *     if false, draw only the negative part.
 */
module OnOffSwitch(baseThickness = 0.8, positiveShape = true) {
    translate([0, HOLDER_WALL_THICKNESS + BODY_MARGIN, baseThickness]) {
        // Base
        if (positiveShape) {
            translate([0, 0, 0])
                cube([BODY_WIDTH, BODY_DEPTH, BODY_HEIGHT]);
        }
        
        // Moving part
        if (positiveShape) {
            translate([-MOVING_PART_SIZE, (BODY_DEPTH - MOVING_PART_SIZE) / 2,
                       (BODY_HEIGHT - MOVING_PART_SIZE) / 2])
                cube([MOVING_PART_SIZE, MOVING_PART_SIZE, MOVING_PART_SIZE]);
        }
        
        // Pins
        if (positiveShape) {
            translate([BODY_WIDTH, (BODY_DEPTH - PINS_DEPTH) / 2, (BODY_HEIGHT - PINS_HEIGHT) / 2])
                cube([PINS_WIDTH, PINS_DEPTH, PINS_HEIGHT]);
        }
    }
}

/**
 * ON-OFF switch holder.
 *
 * @param positiveShape
 *     if true, draw only the positive part.
 *     if false, draw only the negative part.
 */
module OnOffSwitch_holder(baseThickness = 0.8, positiveShape = true) {
    // Base
    baseWidth = OnOffSwitch_holderWidth();
    baseDepth = OnOffSwitch_holderDepth();
    if (positiveShape) {
        cube([baseWidth, baseDepth, baseThickness]);
    }
    
    // Clearance for the moving part
    if (!positiveShape) {
        width = 4;
        depth = BODY_DEPTH - 2;
        height = MOVING_PART_SIZE + 1;
        translate([-width, (baseDepth - depth) / 2, (BODY_HEIGHT - height) / 2 + baseThickness])
            cube([width + EPSILON, depth, height]);
    }
    
    // Clearance for the fixed part (front)
    if (!positiveShape) {
        depth = BODY_DEPTH + 2 * EPSILON;
        height = BODY_HEIGHT + 2 * EPSILON;
        translate([-EPSILON, (baseDepth - depth) / 2, (BODY_HEIGHT - height) / 2 + baseThickness])
            cube([1, depth, height]);
    }
    
    // Top and bottom walls
    wallHeight = BODY_HEIGHT + baseThickness;
    if (positiveShape) {
        horizontalSupportWidth = 1;
        frontClearance = 1;
        wallWidth = baseWidth - frontClearance;
        
        for (y = [0, baseDepth - HOLDER_WALL_THICKNESS])
            translate([frontClearance, y, 0]) {
                cube([wallWidth, HOLDER_WALL_THICKNESS, wallHeight + horizontalSupportWidth]);
                
                translate([0, HOLDER_WALL_THICKNESS / 2, wallHeight])
                    linear_extrude(height = 1, scale = [1, 1.5])
                        translate([0, -HOLDER_WALL_THICKNESS / 2])
                            square([wallWidth - 5, HOLDER_WALL_THICKNESS]);
            }
    }
    
    // Back wall
    if (positiveShape) {
        translate([baseWidth - HOLDER_WALL_THICKNESS, 2, 0]) {
            cube([HOLDER_WALL_THICKNESS, baseDepth - 4, 1.2 + baseThickness]);
        }
    }
}

/**
 * ON-OFF switch clearance.
 *
 * @param positiveShape
 *     if true, draw only the positive part of the switch,
 *     if false, draw only the negative part.
 * @Deprecated To be deleted with FrontCover.scad
 */
module OnOffSwitch_clearance(positiveShape = true) {
    // Moving part clearance
    if (!positiveShape) {
        depth = BODY_DEPTH - 2;
        translate([BODY_WIDTH, -depth - 1, 0.5])
            cube([4, depth, 3]);
    }
}
/*
color("gray")
    difference() {
        OnOffSwitch(0.8, true);
        OnOffSwitch(0.8, false);
    }
    */
difference() {
    union() {
        // Simulate the cover border
        translate([-0.8, 0, 0])
            cube([0.8, OnOffSwitch_holderDepth(), 6]);
        
        OnOffSwitch_holder(0.8, true);
    }
    OnOffSwitch_holder(0.8, false);
}
