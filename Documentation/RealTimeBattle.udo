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

   !node Introduction  
   !label Introduction
   !include Introduction.udo

   !node Usage  
   !label Usage
   !include Usage.udo

   !node Details  
   !label Details
#   !include Details.udo

   !node Robot construction
   !label RobotConstruction
   !include RobotConstruction.udo

   !node Options
   !label Options
   !include Options.udo

   !node Arena construction
   !label ArenaConstruction
   !include ArenaConstruction.udo

#!node History  
#!label History
#!include History.udo

#!node FAQ
#!label FAQ
#!include FAQ.udo

#!node Appendices
#!label Appendices
#!include Appendices.udo

#to get rid of error message in AboutUdo
   !autoref [on] 
   !autoref_items [on]

!end_document

