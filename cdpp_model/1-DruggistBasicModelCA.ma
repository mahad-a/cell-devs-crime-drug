[top]
components : druggistbasicmodel

[druggistbasicmodel]
type : cell
dim : (20,20,1)
delay : transport
defaultDelayTime : 200
border : wrapped 

neighbors : druggistbasicmodel(-2,0,0)  druggistbasicmodel(-1,0,0)  druggistbasicmodel(0,0,0) druggistbasicmodel(1,0,0)  druggistbasicmodel(2,0,0)
neighbors : druggistbasicmodel(0,-2,0)  druggistbasicmodel(0,-1,0)  druggistbasicmodel(0,1,0)  druggistbasicmodel(0,2,0)

zone : nondrugtodrug-transition { (0,0,0)..(19,19,0) }

localtransition : default-rule

initialvalue : 0

[nondrugtodrug-transition]
rule : 1 200 { (0,0,0)=0 AND (0,-1,0)=1 AND (-1,0,0)=1 AND (0,1,0)=1 AND (1,0,0)=1 }
rule : 1 200 { ((0,0,0)=0 OR (0,0,0)=1) AND normal(0.4,0.3) > 0.7 }
rule : 1 200 { (0,0,0)=1 } 
rule : 0 200 { t }

[default-rule]
rule : {(0,0)} 200 {t}  