#Medical extruder print head

This project is based around an prototype print head based on 4 syringes that inject fluids into a mix-chamber. Inside the mix-chamber the fluids blend together into an even substance. Inside the chamber sits a blade that speeds up and improves the blending of the 4 different liquids. The flow of liquids into the chamber is continues, which means that the liquid will eventually exit the chamber through the bottom (extruder).

The code manly focuses on receiving a data stream from the printer which the print-head is mounted on, and converting this data to signals for the linear extruders of the syringes.

  ____  ____  ____  ____
   ][    ][    ][    ][
   ][    ][    ][    ][
   ][    ][    ][    ][
  _][_  _][_  _][_  _][_
  |  |  |  |  |  |  |  |
  |--|  |--|  |--|  |--|
  |  |  |  |  |  |  |  |    <4x syringes with lurelock connection
  |  |  |  |  |  |  |  |    <6cm filled with 3 ml of gel
  |  |  |  |  |  |  |  |
  |  |  |  |  |  |  |  |
   \/    \/    \/    \/
   [________  ________]
           |  |
         __|  |__
       //        \\
      ||   <-->   ||  <inside mix-chamber mix-blade 0-320 RPM
      ||  <---->  ||  <mix-chamber
      \\   <-->   //
       \\__    __//
           |  |       <extruder
