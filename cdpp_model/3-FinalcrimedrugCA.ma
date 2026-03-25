[top]
components : crimedrugCA

[crimedrugCA]
type : cell
dim : (20,20,4)
delay : transport
defaultDelayTime : 100
border : wrapped 

neighbors : crimedrugCA(0,0,-3) crimedrugCA(0,0,-2)  crimedrugCA(0,0,-1)  crimedrugCA(0,0,0)  crimedrugCA(0,0,1) crimedrugCA(0,0,2) crimedrugCA(0,0,3)
neighbors : crimedrugCA(0,-1,-1) crimedrugCA(0,1,-1) crimedrugCA(-1,0,-1) crimedrugCA(1,0,-1)
neighbors : crimedrugCA(-2,0,0)  crimedrugCA(-1,0,0)  crimedrugCA(1,0,0)  crimedrugCA(2,0,0)
neighbors : crimedrugCA(0,-2,0)  crimedrugCA(0,-1,0)  crimedrugCA(0,1,0)  crimedrugCA(0,2,0)

zone : LRP-transition { (0,0,0)..(19,19,0) }
zone : HRP-transition { (0,0,1)..(19,19,1) }
zone : crimedrug-transition { (0,0,2)..(19,19,2) }
zone : Incapacitation-transition { (0,0,3)..(19,19,3) }

localtransition : default-rule

initialvalue : 0

[LRP-transition]
rule : 1 100 { (0,0,0)=0 AND (0,-1,0)=1 AND (-1,0,0)=1 AND (0,1,0)=1 AND (1,0,0)=1 }
rule : 1 100 { (0,0,0)=0 AND normal(0.4,0.3) > 0.6 }
rule : 1 100 { (0,0,0)=1 } 
rule : 0 100 { t }

[HRP-transition]
rule : 2 100 { (0,0,0)=0 AND (0,0,-1)=1 AND (0,-1,-1)=1 AND (-1,0,-1)=1 AND (0,1,-1)=1 AND (1,0,-1)=1 }
rule : 2 100 { (0,0,0)=2 }
rule : 0 100 { t }

[crimedrug-transition] 
rule : 3 100 { (0,0,0)=0 AND normal(0.4,0.1) > 0.6 }
rule : 3 100 { (0,0,0)=0 AND (0,0,-1)=2 AND ((0,-1,-1)=2 OR (-1,0,-1)=2 OR (0,1,-1)=2 OR (1,0,-1)=2) }
rule : 3 100 { (0,0,0)=3 }
rule : 0 100 { t }

[Incapacitation-transition]
rule : 4 100 { (0,0,0)=0 AND (0,0,-1)=3 AND (0,0,-2)=2 AND normal(0.4,0.3) > 0.1 }
rule : 4 100 { (0,0,0)=4 }
rule : 0 100 { t }

[default-rule]
rule : {(0,0)} 100 {t}  
