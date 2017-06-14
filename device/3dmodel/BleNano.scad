EPSILON = 0.001;

BLE_NANO_WIDTH = 18.5;
BLE_NANO_DEPTH = 21;
BLE_NANO_PCB_HEIGHT = 1;
PCB_CORNER_RADIUS = 1.5;

FRONT_COMPONENTS_HEIGHT = 1;
BACK_COMPONENTS_HEIGHT = 3;

HOLDER_WALL_THICKNESS = 0.8;
BLE_NANO_MARGIN = 0.1;

function BleNano_width() = BLE_NANO_WIDTH;
function BleNano_depth() = BLE_NANO_DEPTH;
function BleNano_holderWidth() = BLE_NANO_WIDTH + 2 * (HOLDER_WALL_THICKNESS + BLE_NANO_MARGIN);
function BleNano_holderDepth() = BLE_NANO_DEPTH + 2 * (HOLDER_WALL_THICKNESS + BLE_NANO_MARGIN);

/**
 * BLE nano microcontroller.
 *
 * @param positiveShape
 *     if true, draw only the positive part.
 *     if false, draw only the negative part.
 */
module BleNano(baseThickness = 0.8, positiveShape = true) {
    // Base
    baseX = HOLDER_WALL_THICKNESS + BLE_NANO_MARGIN;
    baseY = HOLDER_WALL_THICKNESS + BLE_NANO_MARGIN;
    if (positiveShape) {
        translate([baseX, baseY, BACK_COMPONENTS_HEIGHT + baseThickness])
            linear_extrude(height = BLE_NANO_PCB_HEIGHT)
                offset(r = PCB_CORNER_RADIUS, $fn = 32)
                    offset(r = -PCB_CORNER_RADIUS, $fn = 32)
                        square([BLE_NANO_WIDTH, BLE_NANO_DEPTH]);
    }
    
    // Front components
    if (positiveShape) {
        width = 15;
        depth = 11.5;
        translate([baseX - (width - BLE_NANO_WIDTH) / 2,
                   baseY + 3,
                   BACK_COMPONENTS_HEIGHT + BLE_NANO_PCB_HEIGHT + baseThickness])
            cube([width, depth, FRONT_COMPONENTS_HEIGHT]);
    }
    
    // Back components
    if (positiveShape) {
        width = 15;
        depth = 17.25;
        translate([baseX - (width - BLE_NANO_WIDTH) / 2, baseY + 1, baseThickness])
            cube([width, depth, BACK_COMPONENTS_HEIGHT]);
    }
}

/**
 * BLE nano holder.
 *
 * @param positiveShape
 *     if true, draw only the positive part.
 *     if false, draw only the negative part.
 */
module BleNano_holder(baseThickness = 0.8, positiveShape = true) {
    // Base
    baseWidth = BleNano_holderWidth();
    baseDepth = BleNano_holderDepth();
    if (positiveShape) {
        linear_extrude(height = baseThickness)
            offset(r = PCB_CORNER_RADIUS, $fn = 32)
                offset(r = -PCB_CORNER_RADIUS, $fn = 32)
                    square([baseWidth, baseDepth]);
    }
    
    // Supports
    if (positiveShape) {
        wallHeight = baseThickness + BACK_COMPONENTS_HEIGHT + baseThickness;
        horizontalTopSupportHeight = 1;
        horizontalTopSupportWidth = 0.6;
        innerClearanceWidth = BLE_NANO_WIDTH + 2 * BLE_NANO_MARGIN;
        innerClearanceDepth = BLE_NANO_DEPTH + 2 * BLE_NANO_MARGIN;
        
        difference() {
            union() {
                // Contour around the BLE Nano
                linear_extrude(height = wallHeight)
                    difference() {
                        offset(r = PCB_CORNER_RADIUS, $fn = 32)
                            offset(r = -PCB_CORNER_RADIUS, $fn = 32)
                                square([baseWidth, baseDepth]);
                
                        translate([HOLDER_WALL_THICKNESS, HOLDER_WALL_THICKNESS])
                            offset(r = PCB_CORNER_RADIUS, $fn = 32)
                                offset(r = -PCB_CORNER_RADIUS, $fn = 32)
                                    square([innerClearanceWidth, innerClearanceDepth]);
                    }
                    
                // Horizontal supports on top of the BLE Nano
                translate([0, 0, wallHeight])
                    difference() {
                        linear_extrude(height = horizontalTopSupportHeight)
                            offset(r = PCB_CORNER_RADIUS, $fn = 32)
                                offset(r = -PCB_CORNER_RADIUS, $fn = 32)
                                    square([baseWidth, baseDepth]);
                
                        scale = [(innerClearanceWidth - 2 * horizontalTopSupportWidth) / 
                                 innerClearanceWidth,
                                 (innerClearanceDepth - 2 * horizontalTopSupportWidth) / 
                                 innerClearanceDepth];
                        translate([HOLDER_WALL_THICKNESS, HOLDER_WALL_THICKNESS, -EPSILON])
                            translate([innerClearanceWidth / 2, innerClearanceDepth / 2])
                                linear_extrude(height = horizontalTopSupportHeight + 2 * EPSILON,
                                               scale = scale)
                                    translate([-innerClearanceWidth / 2, -innerClearanceDepth / 2])
                                        offset(r = PCB_CORNER_RADIUS, $fn = 32)
                                            offset(r = -PCB_CORNER_RADIUS, $fn = 32)
                                                square([innerClearanceWidth, innerClearanceDepth]);
                    }
            }
            
            // Remove the support corners
            cornerSize = HOLDER_WALL_THICKNESS + horizontalTopSupportWidth + 2 + 2 * EPSILON;
            cornerheight = wallHeight + horizontalTopSupportHeight + EPSILON;
            for (y = [-EPSILON, baseDepth - cornerSize + EPSILON])
                for (x = [-EPSILON, baseWidth - cornerSize + EPSILON])
                    translate([x, y, 0])
                        cube([cornerSize, cornerSize, cornerheight]);
            
            // Clearance for the surface pins
            translate([-EPSILON, -EPSILON, wallHeight - 0.5])
                cube([baseWidth + 2 * EPSILON,
                      HOLDER_WALL_THICKNESS + horizontalTopSupportWidth + 2 * EPSILON,
                      horizontalTopSupportHeight + 0.5 + EPSILON]);
        }
    }
}
/*
color("red")
    difference() {
        BleNano(0.8, true);
        BleNano(0.8, false);
    }
*/
difference() {
    BleNano_holder(0.8, true);
    BleNano_holder(0.8, false);
}