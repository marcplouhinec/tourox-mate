EPSILON = 0.001;

FONA_BASE_WIDTH = 1.25 * 25.4;
FONA_BASE_DEPTH = 1.75 * 25.4;
FONA_BASE_HEIGHT = 1.80;

FONA_SIM_CARD_HOLDER_WIDTH = 18;
FONA_SIM_CARD_HOLDER_DEPTH = 30;
FONA_SIM_CARD_HOLDER_HEIGHT = 2.4;
FONA_SIM_CARD_HOLDER_MARGIN_TOP = 8;

FONA_HOLDER_WALL_THICKNESS = 0.8;
FONA_MARGIN = 0.4;

function Fona_width() = FONA_BASE_WIDTH;
function Fona_depth() = FONA_BASE_DEPTH;

function Fona_holderWidth() = FONA_BASE_WIDTH + 2 * (FONA_HOLDER_WALL_THICKNESS + FONA_MARGIN);
function Fona_holderDepth() = FONA_BASE_DEPTH + 2 * (FONA_HOLDER_WALL_THICKNESS + FONA_MARGIN);

/**
 * Adafruit Fona.
 *
 * @param positiveShape
 *     if true, draw only the positive part of the holder,
 *     if false, draw only the negative part.
 */
module Fona(baseThickness = 0.8, positiveShape = true) {
    // Base
    baseX = FONA_HOLDER_WALL_THICKNESS + FONA_MARGIN;
    baseY = FONA_HOLDER_WALL_THICKNESS + FONA_MARGIN;
    baseZ = baseThickness;
    if (positiveShape) {
        cornerRadius = 0.1 * 25.4;
        translate([baseX, baseY, FONA_SIM_CARD_HOLDER_HEIGHT + baseZ])
            linear_extrude(height = FONA_BASE_HEIGHT)
                offset(r = cornerRadius, $fn = 32)
                    offset(r = -cornerRadius, $fn = 32)
                        square([FONA_BASE_WIDTH, FONA_BASE_DEPTH]);
    }
    
    // Holes
    if (!positiveShape) {
        translate([baseX, baseY + FONA_BASE_DEPTH, FONA_SIM_CARD_HOLDER_HEIGHT - EPSILON + baseZ])
            Fona_holes(FONA_BASE_HEIGHT + 2 * EPSILON, false);
    }
    
    // Connectors
    if (positiveShape) {
        translate([baseX, baseY + FONA_BASE_DEPTH, FONA_SIM_CARD_HOLDER_HEIGHT + FONA_BASE_HEIGHT + baseZ])
            Fona_connectors(false);
    }
    
    // SIM card holder
    if (positiveShape) {
        translate([baseX + FONA_BASE_WIDTH - FONA_SIM_CARD_HOLDER_WIDTH,
                   baseY + FONA_BASE_DEPTH - FONA_SIM_CARD_HOLDER_DEPTH - FONA_SIM_CARD_HOLDER_MARGIN_TOP,
                   baseZ])
            cube([FONA_SIM_CARD_HOLDER_WIDTH, FONA_SIM_CARD_HOLDER_DEPTH, FONA_SIM_CARD_HOLDER_HEIGHT]);
    }
}

/**
 * Fona holder and wall clearance.
 *
 * @param positiveShape
 *     if true, draw only the positive part of the holder,
 *     if false, draw only the negative part.
 */
module Fona_holder(baseThickness = 0.8, positiveShape = true) {
    // Base
    baseWidth = Fona_holderWidth();
    baseDepth = Fona_holderDepth();
    if (positiveShape)
        linear_extrude(height = baseThickness)
            offset(r = 4, $fn = 32)
                offset(r = -4, $fn = 32)
                    square([baseWidth, baseDepth]);
    
    // Pillars
    fonaX = FONA_HOLDER_WALL_THICKNESS + FONA_MARGIN;
    fonaY = FONA_HOLDER_WALL_THICKNESS + FONA_MARGIN;
    fonaBaseZ = FONA_SIM_CARD_HOLDER_HEIGHT + baseThickness;
    if (positiveShape) {
        translate([fonaX, fonaY + FONA_BASE_DEPTH, 0])
            Fona_holes(fonaBaseZ + FONA_BASE_HEIGHT + 0.8, true);
    }
    
    // Top and bottom walls
    if (positiveShape) {
        wallWidth = 24;
        for (y = [0, baseDepth - FONA_HOLDER_WALL_THICKNESS])
            translate([(baseWidth - wallWidth) / 2, y, 0])
                cube([wallWidth, FONA_HOLDER_WALL_THICKNESS, fonaBaseZ + FONA_BASE_HEIGHT]);
    }
    
    // Left and right walls
    if (positiveShape) {
        wallDepth = 24;
        horizontalHolderWidth = FONA_HOLDER_WALL_THICKNESS;
        horizontalHolderHeight = fonaBaseZ + FONA_BASE_HEIGHT + FONA_HOLDER_WALL_THICKNESS;
        wallHeight = fonaBaseZ + FONA_BASE_HEIGHT + horizontalHolderWidth;
        
        for (x = [0, baseWidth])
            translate([x, (baseDepth - wallDepth) / 2 + (x > 0 ? wallDepth : 0), 0])
                rotate([0, 0, x > 0 ? 180 : 0]){
                    cube([FONA_HOLDER_WALL_THICKNESS, wallDepth, wallHeight]);
                
                    translate([horizontalHolderWidth - EPSILON, 0, horizontalHolderHeight])
                        rotate([-90, 0, 0])
                            linear_extrude(height = wallDepth)
                                polygon(points = [[0, 0],
                                                 [horizontalHolderWidth, 0],
                                                 [0, horizontalHolderWidth + 0.2]]);
                }
    }
    
    // Connector clearance
    if (!positiveShape) {
        translate([fonaX, fonaY + FONA_BASE_DEPTH, FONA_BASE_HEIGHT + fonaBaseZ - 0.2]) {
            Fona_connectors(clearance = true, includeJstConnector = false);
            translate([6 - FONA_HOLDER_WALL_THICKNESS - 0.2, 0, 0])
                Fona_connectors(clearance = true, includeJstConnector = true);
        }
    }
}

/**
 * Holes of the FONA.
 *
 * @param positiveShape
 *     if true, draw only the positive part of the holder,
 *     if false, draw only the negative part.
 */
module Fona_holes(height = 10, positiveShape = true) {
    margin = positiveShape ? 0.2 : 0;
    distanceBetweenHoles = 1.05 * 25.4;
    
    for (x = [0, distanceBetweenHoles])
        translate([(FONA_BASE_WIDTH - distanceBetweenHoles) / 2 + x, -0.1 * 25.4])
            cylinder(d = 0.1 * 25.4 - margin, h = height, $fn = 32);
}

/**
 * FONA connectors.
 *
 * @param positiveShape
 *     if true, draw only the positive part of the holder,
 *     if false, draw only the negative part.
 */
module Fona_connectors(clearance = false, includeJstConnector = true) {
    translate([clearance ? -6 : 0, 0, 0]) {
        // 2-pin JST-PH connector
        if (includeJstConnector) {
            distanceBottomJstConnector = 1.38 * 25.4;
            jstConnectorWidth = 8.5;
            jstConnectorHeight = 7.3 - FONA_BASE_HEIGHT;
            translate([jstConnectorWidth / 2, -FONA_BASE_DEPTH + distanceBottomJstConnector, jstConnectorHeight / 2])
                cube(size = [jstConnectorWidth, 8.8, jstConnectorHeight], center = true);
        }
        
        // USB connector
        distanceBottomUsbConnector = 0.87 * 25.4;
        usbConnectorWidth = 6.2;
        usbConnectorHeight = 5.4 - FONA_BASE_HEIGHT;
        translate([usbConnectorWidth / 2, -FONA_BASE_DEPTH + distanceBottomUsbConnector, usbConnectorHeight / 2])
            cube(size = [usbConnectorWidth, 8.8, usbConnectorHeight], center = true);
        
        // Headphone connector
        distanceBottomHeadphoneConnector = 0.44 * 25.4;
        headConnectorWidth = 12.8 + 1.4;
        headConnectorHeight = 7.2 - FONA_BASE_HEIGHT;
        translate([headConnectorWidth / 2 - 1.4, -FONA_BASE_DEPTH + distanceBottomHeadphoneConnector, headConnectorHeight / 2])
            cube(size = [headConnectorWidth, 7.1, headConnectorHeight], center = true);
    }
}
/*
color("dimgray")
difference() {
    Fona(baseThickness = 0.8, positiveShape = true);
    Fona(baseThickness = 0.8, positiveShape = false);
}
*/
difference() {
    Fona_holder(baseThickness = 0.8, positiveShape = true);
    Fona_holder(baseThickness = 0.8, positiveShape = false);
}
