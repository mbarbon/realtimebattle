!language english
!title RealTimeBattle User Manual
!version Version 0.1
!date (!today)
!author Erik Ouchterlony and Ragnar Ouchterlony
!email ragou603@student.liu.se

!code_iso
!html_merge_subnodes

!autoref [off]
!autoref_items [off]

!begin_document

   !maketitle
   !tableofcontents

   !label Introduction
   !node Introduction  
   !include Introduction.udo

   !label Usage
   !node Usage  
   !include Usage.udo

   !label Structure
   !node Structure
   !include Structure.udo

   !label RobotConstruction
   !node Robot construction
   !include RobotConstruction.udo

   !label Options
   !node Options
   !include Options.udo

   !label ArenaConstruction
   !node Arena construction
   !include ArenaConstruction.udo

#!label History
#!node History  
#!include History.udo

#!label FAQ
#!node FAQ
#!include FAQ.udo

#!label Appendices
#!node Appendices
#!include Appendices.udo

#to get rid of error message in AboutUdo
   !autoref [on] 
   !autoref_items [on]

!end_document

