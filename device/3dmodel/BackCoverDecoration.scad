use <Box.scad>;
use <Fona.scad>;
use <OnOffSwitch.scad>;
use <Battery1200mAh.scad>;
use <BleNano.scad>;
use <PushButtons.scad>;
use <Lanyard.scad>;

EPSILON = 0.001;

/**
 * Supports, holders and clearances for the back cover.
 *
 * @param positiveShape
 *     if true, draw only the positive part.
 *     if false, draw only the negative part.
 * @param includeComponents
 *     if true, include the components like the battery, switch, ...etc.
 */
module BackCoverDecoration(positiveShape = true, includeComponents = true) {
    // FONA
    fonaYPos = Box_outerCoverDepth() - Box_coverThickness() - Fona_holderDepth() - 5.4;
    translate([Box_coverThickness() - EPSILON + 0.3, fonaYPos, 0]) {
        if (includeComponents)
            color("blue")
                Fona(Box_coverThickness(), positiveShape);
        
        Fona_holder(Box_coverThickness(), positiveShape);
    }
    
    // BLE Nano
    translate([Box_outerCoverWidth() - Box_coverThickness() - BleNano_holderWidth() - 0.4,
               Box_outerCoverDepth() - Box_coverThickness() - BleNano_holderDepth() - 18,
               0]) {
        if (includeComponents)
            color("red")
                BleNano(Box_coverThickness(), positiveShape);
        
        // Note: the support is not necessary since the BLE Nano is already
        // constrained by all its connected wires.
        //BleNano_holder(Box_coverThickness(), positiveShape);
    }
    
    // Battery
    translate([(Box_outerCoverWidth() - Battery1200mAh_holderWidth()) / 2, 6.95, 0]) {
        if (includeComponents)
            color("orange")
                Battery1200mAh(Box_coverThickness(), positiveShape);
        
        Battery1200mAh_holder(Box_coverThickness(), positiveShape);
    }
    
    // On-off switch
    translate([Box_coverThickness() + 0.2, fonaYPos - OnOffSwitch_depth() - 16, 0]) {
        baseThickness = Box_coverThickness() + 4.7;
        
        if (includeComponents)
            color("gray")
                OnOffSwitch(baseThickness, positiveShape);
        OnOffSwitch_holder(baseThickness, positiveShape);
    }
    
    // Push buttons
    translate([Box_outerCoverWidth() / 2, 17, 0]) {
        distanceBetweenButtons = 43;
        buttonBase = Box_coverThickness() + 8.2;
        
        if (includeComponents)
            color("brown")
                PushButtons(buttonBase, distanceBetweenButtons, positiveShape);

        PushButtons_holder(buttonBase, distanceBetweenButtons, positiveShape);
    }
    
    // Lanyard holder
    if (positiveShape) {
        intersection() {
            translate([Box_outerCoverWidth() / 2, 1, 0])
                Lanyard_holder(Box_coverThickness() + 0.8, positiveShape);
                
            Box_cover(inner = false, height = Box_backOuterCoverHeight(), fill = true);
        }
    } else {
        translate([Box_outerCoverWidth() / 2, 1, 0])
                Lanyard_holder(Box_coverThickness() + 0.8, positiveShape);
    }
}


includeComponents = true;
difference() {
    union() {
        Box_backCover();
        BackCoverDecoration(positiveShape = true, includeComponents = includeComponents);
    }
    BackCoverDecoration(positiveShape = false, includeComponents = includeComponents);
}