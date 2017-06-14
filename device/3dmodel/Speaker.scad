EPSILON = 0.001;
SPEAKER_RADIUS = 14;
SPEAKER_HOLDER_THICKNESS = 0.8;
SPEAKER_HOLDER_HEIGHT = 3;
SPEAKER_HOLDER_GAP = 0.1;
BASE_THICKNESS_UNDER_SPEAKER = 0.2;

function Speaker_size() = 2 * (SPEAKER_RADIUS + SPEAKER_HOLDER_THICKNESS);

/**
 * Speaker holder for the front cover.
 *
 * @param positiveShape
 *     if true, draw only the positive part.
 *     if false, draw only the negative part.
 */
module Speaker_holder(baseThickness = 0.8, positiveShape = true) {
    speakerPosition = [SPEAKER_RADIUS + SPEAKER_HOLDER_THICKNESS,
                       SPEAKER_RADIUS + SPEAKER_HOLDER_THICKNESS,
                       baseThickness - EPSILON];
    
    // Base
    if (positiveShape)
        translate([Speaker_size() / 2 , Speaker_size() / 2, 0])
            cylinder(h = baseThickness, d = Speaker_size(), $fn = 128);
    
    // Speaker holder
    if (positiveShape) {
        translate(speakerPosition)
            difference() {
                // Filled holder
                cylinder(h = SPEAKER_HOLDER_HEIGHT,
                         r = SPEAKER_RADIUS + SPEAKER_HOLDER_THICKNESS, $fn = 128);
                
                // Holder clearance (from root to middle)
                translate([0, 0, 0])
                    cylinder(h = SPEAKER_HOLDER_HEIGHT / 2,
                             r = SPEAKER_RADIUS + SPEAKER_HOLDER_GAP, $fn = 128);
                
                // Holder clearance (from middle to top)
                translate([0, 0, SPEAKER_HOLDER_HEIGHT / 2 - EPSILON])
                    linear_extrude(height = SPEAKER_HOLDER_HEIGHT / 2 + EPSILON * 2,
                                   scale=[0.96, 0.96], $fn = 128)
                        circle(r = SPEAKER_RADIUS + SPEAKER_HOLDER_GAP);
                
                // Clearance in the middle of the holder to allow the speaker to be inserted
                width = SPEAKER_RADIUS * 2 + 2;
                depth = 24;
                translate([-width / 2, -depth / 2, 2 * EPSILON])
                    cube([width, depth, SPEAKER_HOLDER_HEIGHT + 1]);
            }
    }
    
    // Make some clearance under the speaker to make the base thiner
    if (!positiveShape) {
        translate([speakerPosition[0],
                   speakerPosition[1],
                   speakerPosition[2] - baseThickness + BASE_THICKNESS_UNDER_SPEAKER])
            cylinder(h = SPEAKER_HOLDER_HEIGHT / 2 + baseThickness - BASE_THICKNESS_UNDER_SPEAKER,
                     r = SPEAKER_RADIUS + SPEAKER_HOLDER_GAP, $fn = 128);
    }
    
    // Speaker holes
    if (!positiveShape) {
        width = 20;
        depth = 1;
        height = baseThickness + 2 * EPSILON;
        
        for (ypos = [-5, 0, 5]) {
            translate([
                speakerPosition[0] - width / 2,
                speakerPosition[1] - depth / 2 + ypos,
                speakerPosition[2] - baseThickness
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

difference() {
    union() {
        Speaker_holder(0.8, true);
        translate([-5, -5, 0])
        linear_extrude(height = 0.8)
            offset(r = 10, $fn = 64)
            offset(r = -10, $fn = 64)
                square(40, 40);
    }
    Speaker_holder(0.8, false);
}