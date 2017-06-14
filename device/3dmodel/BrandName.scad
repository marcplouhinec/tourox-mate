BRAND_NAME_WIDTH = 43;
BRAND_NAME_DEPTH = 14;
BRAND_NAME_CORNER_RADIUS = 2;

NORMAL_BRAND_NAME_HEIGHT = 0.2;
COLORED_BRAND_NAME_MARGIN_LEFT = 2;
COLORED_BRAND_NAME_MARGIN_TOP = 2;
COLORED_BRAND_NAME_POSITION = [
    COLORED_BRAND_NAME_MARGIN_LEFT,
    -BRAND_NAME_DEPTH + COLORED_BRAND_NAME_MARGIN_TOP,
    NORMAL_BRAND_NAME_HEIGHT
];
COLORED_BRAND_NAME_DIMENSION = [
    BRAND_NAME_WIDTH - 2 * COLORED_BRAND_NAME_MARGIN_LEFT,
    BRAND_NAME_DEPTH - 2 * COLORED_BRAND_NAME_MARGIN_TOP
];

TEXT_MARGIN_TOP = 3;
TEXT_MARGIN_LEFT = 3;
TEXT_POSITION = [38 + TEXT_MARGIN_LEFT, -7.6 - TEXT_MARGIN_TOP];

function BrandName_width() = BRAND_NAME_WIDTH;
function BrandName_depth() = BRAND_NAME_DEPTH;

/**
 * Brand name.
 * 
 * @param height height of the brand name.
 * @param positiveShape
 *     if true, draw only the positive part of the brand name,
 *     if false, draw only the negative part.
 */
module BrandName_frame(height = 1, positiveShape = true) {
    // Base
    if (positiveShape) {
        translate([0, -BRAND_NAME_DEPTH, 0])
            linear_extrude(height = height)
                offset(r = BRAND_NAME_CORNER_RADIUS, $fn = 32)
                    offset(r = -BRAND_NAME_CORNER_RADIUS, $fn = 32)
                        square([BRAND_NAME_WIDTH, BRAND_NAME_DEPTH]);
    }
    
    // Text hole
    if (!positiveShape) {
        translate([TEXT_POSITION[0], TEXT_POSITION[1], height * 2 - NORMAL_BRAND_NAME_HEIGHT])
            rotate([0, 180, 0])
                linear_extrude(height = height * 2)
                    _BrandName_text();
    }
    
    // Hole for the colored part
    if (!positiveShape) {
        translate(COLORED_BRAND_NAME_POSITION)
            linear_extrude(height = height)
                offset(r = BRAND_NAME_CORNER_RADIUS, $fn = 32)
                    offset(r = -BRAND_NAME_CORNER_RADIUS, $fn = 32)
                        square(COLORED_BRAND_NAME_DIMENSION);
    }
    
    // Colored part support
    if (positiveShape) {
        width = 2;
        rounding = 0.3;
        depth = BRAND_NAME_DEPTH - 2 * BRAND_NAME_CORNER_RADIUS;
        xpositions = [
            COLORED_BRAND_NAME_MARGIN_LEFT,
            BRAND_NAME_WIDTH - COLORED_BRAND_NAME_MARGIN_LEFT
        ];
        wallHeight = 0.41;
        
        for (x = xpositions)
            translate([
                x,
                (x == xpositions[0] ? -depth : 0) - BRAND_NAME_CORNER_RADIUS,
                height + width + wallHeight + 0.1
            ]) {
                minkowski() {
                    rotate([0, 0, x == xpositions[0] ? 0 : 180]) {
                        translate([-rounding, 0, 0])
                            rotate([-90, 0, 0])
                                linear_extrude(height = depth)
                                    polygon(points = [[0, 0], [width, 0], [0, width * 1.5]]);
                        
                        translate([
                            -COLORED_BRAND_NAME_MARGIN_LEFT + rounding,
                            0,
                            -width - wallHeight
                        ]) {
                            cube([
                                COLORED_BRAND_NAME_MARGIN_LEFT - 2* rounding,
                                depth,
                                width + wallHeight
                            ]);
                        }
                    }
                    sphere(r = rounding, $fn = 32);
                }
            }
    }
}

/**
 * Colored part of the brand name.
 * 
 * @param height height of the brand name.
 * @param positiveShape
 *     if true, draw only the positive part of the brand name,
 *     if false, draw only the negative part.
 */
module BrandName_coloredBrandName(height = 1, positiveShape = true) {
    verticalFittingGap = 0.1;
    horizontalFittingGap = 0.2;
    
    // Text
    if (positiveShape) {
        translate([TEXT_POSITION[0], TEXT_POSITION[1], height - verticalFittingGap])
            rotate([0, 180, 0])
                linear_extrude(height = height + verticalFittingGap)
                    offset(r = -0.2, $fn = 32)
                        _BrandName_text();
    }
    
    // Base
    if (positiveShape) {
        translate([
            COLORED_BRAND_NAME_POSITION[0] + horizontalFittingGap,
            COLORED_BRAND_NAME_POSITION[1] + horizontalFittingGap,
            COLORED_BRAND_NAME_POSITION[2]
        ]) {
            linear_extrude(height = height - verticalFittingGap - NORMAL_BRAND_NAME_HEIGHT)
                offset(r = BRAND_NAME_CORNER_RADIUS, $fn = 32)
                    offset(r = -BRAND_NAME_CORNER_RADIUS, $fn = 32)
                        square([
                            COLORED_BRAND_NAME_DIMENSION[0] - 2 * horizontalFittingGap,
                            COLORED_BRAND_NAME_DIMENSION[1] - 2 * horizontalFittingGap
                        ]);
        }
    }
}

module _BrandName_text(bigSize = false) {
    if (bigSize)
        difference() {
            text(text = "TOUROX", font = "Hemi Head Rg", size = 8);
            for (x = [0, -14.8, 7])
                translate([22 + x, 5.28])
                    square([2.5, 0.8]);
        }
    else
        difference() {
            text(text = "TOUROX", font = "Hemi Head Rg", size = 7);
            for (x = [0, -12.7, 6.4])
                translate([19.1 + x, 4.51])
                    square([2, 0.8]);
        }
}


difference() {
    BrandName_frame(1, true);
    BrandName_frame(1, false);
}
/*
//rotate([0, 180, 0])
color("red")
    difference() {
        BrandName_coloredBrandName(1, true);
        BrandName_coloredBrandName(1, false);
    }*/