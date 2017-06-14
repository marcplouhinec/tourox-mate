use <Box.scad>;
use <Speaker.scad>;
use <BrandName.scad>;
use <Screen.scad>;
use <Microphone.scad>;
use <PushButtons.scad>;

/**
 * Supports, holders and clearances for the front cover.
 *
 * @param positiveShape
 *     if true, draw only the positive part.
 *     if false, draw only the negative part.
 */
module FrontCoverDecoration(positiveShape = true) {
    // Speaker
    speakerHolderYPos = Box_outerCoverDepth() - Speaker_size() - 3.6;
    translate([(Box_outerCoverWidth() - Speaker_size()) / 2, speakerHolderYPos, 0])
        Speaker_holder(Box_coverThickness(), positiveShape);
    
    // Brand name
    brandNameYPos = speakerHolderYPos + 1;
    translate([(Box_outerCoverWidth() - BrandName_width()) / 2, brandNameYPos, 0])
        BrandName_frame(Box_coverThickness(), positiveShape);
    
    // Screen
    screenHolderYPos = brandNameYPos - BrandName_depth();
    translate([(Box_outerCoverWidth() - Screen_holderWidth()) / 2, screenHolderYPos, 0])
        Screen_holder(Box_coverThickness(), positiveShape);

    // Push button cover
    pushButtonCoverWidth = 50;
    translate([(Box_outerCoverWidth() + pushButtonCoverWidth) / 2, 5, Box_coverThickness()])
        rotate([0, 180, 0])
            PushButtons_cover(baseThickness = Box_coverThickness(),
                          width = pushButtonCoverWidth,
                          depth = 19.8,
                          positiveShape = positiveShape);
    
    // Microphone
    translate([Box_outerCoverWidth() / 2,
               Microphone_holderDiameter() / 2 + Box_coverThickness() + 1.2,
               0])
        Microphone_holder(Box_coverThickness(), positiveShape);
}

difference() {
    union() {
        Box_frontCover();
        FrontCoverDecoration(positiveShape = true);
    }
    FrontCoverDecoration(positiveShape = false);
}

/*
rotate([180, 0, 0])
difference() {
    BrandName_coloredBrandName(Box_coverThickness(), true);
    BrandName_coloredBrandName(Box_coverThickness(), false);
}*/