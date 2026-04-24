window.BENCHMARK_DATA = {
  "lastUpdate": 1777001704987,
  "repoUrl": "https://github.com/viktorika/jitfusion",
  "entries": {
    "JitFusion Engine Benchmark": [
      {
        "commit": {
          "author": {
            "email": "598855847@qq.com",
            "name": "Weiqiang Wu",
            "username": "viktorika"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "3780ab682c973d0b463866b60a4b80a190b78822",
          "message": "Merge pull request #221 from viktorika/dev\n\nfix benchmark",
          "timestamp": "2026-04-24T10:31:54+08:00",
          "tree_id": "cb39e1f1ac734ce0555b6031128e56953184661c",
          "url": "https://github.com/viktorika/jitfusion/commit/3780ab682c973d0b463866b60a4b80a190b78822"
        },
        "date": 1776999070687,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_Compile_MinimalExpr_mean",
            "value": 2.5107845603603383,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5106834630630632 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_MinimalExpr_median",
            "value": 2.495297756756888,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.4952838108108106 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_MinimalExpr_stddev",
            "value": 0.03650314058393758,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.036470826869201514 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_MinimalExpr_cv",
            "value": 0.014538539530726917,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.01452625446646575 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/10_mean",
            "value": 2.5049036954956425,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5048037117117117 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/10_median",
            "value": 2.5038538288290386,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.503833720720721 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/10_stddev",
            "value": 0.0037294589810390128,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0037189237186287852 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/10_cv",
            "value": 0.0014888632196700358,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0014847166271912695 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/100_mean",
            "value": 2.548533032727185,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.548221905454544 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/100_median",
            "value": 2.5470083454546386,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5469899909090907 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/100_stddev",
            "value": 0.006029700943903062,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.006157286085237539 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/100_cv",
            "value": 0.002365949692027605,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0024163068656060474 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/1000_mean",
            "value": 2.9948696297872948,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.9945731765957495 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/1000_median",
            "value": 2.988453680850906,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.9884331702127724 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/1000_stddev",
            "value": 0.013606119310213906,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.013679834349167419 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/1000_cv",
            "value": 0.0045431424376159765,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.004568208403148373 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/10_mean",
            "value": 2.5447146181818425,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.544361983636363 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/10_median",
            "value": 2.542836436363545,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5427235818181755 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/10_stddev",
            "value": 0.007161022280325166,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.006996866273154359 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/10_cv",
            "value": 0.0028140767648993197,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0027499492281969034 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/50_mean",
            "value": 2.710378548571433,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.7102241466666634 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/50_median",
            "value": 2.6794512380953552,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.6794289047618998 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/50_stddev",
            "value": 0.04921881201644716,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.04910268360517008 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/50_cv",
            "value": 0.01815938664449254,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.018117572919406702 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/200_mean",
            "value": 3.213796850000057,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 3.2136308068181862 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/200_median",
            "value": 3.213399295454167,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 3.2133715454545353 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/200_stddev",
            "value": 0.0046710747883033,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.004649385880662026 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/200_cv",
            "value": 0.0014534443234342016,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0014467703853216982 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/10_mean",
            "value": 2.530045027272598,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5299214181818193 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/10_median",
            "value": 2.5291357909090744,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.529042863636361 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/10_stddev",
            "value": 0.0032560157486133624,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.003236905610251932 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/10_cv",
            "value": 0.0012869398423803408,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0012794490718127528 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/100_mean",
            "value": 2.6759866457143198,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.675883718095242 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/100_median",
            "value": 2.676233133333231,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.6761158571428636 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/100_stddev",
            "value": 0.0055337423305213875,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.005562339850317111 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/100_cv",
            "value": 0.00206792598886241,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0020786926624287386 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ListOps_mean",
            "value": 2.7157402155338994,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.7156376970873723 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ListOps_median",
            "value": 2.715499533980544,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.7153736310679566 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ListOps_stddev",
            "value": 0.004928669110295236,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.004848066530376262 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ListOps_cv",
            "value": 0.0018148529384745613,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0017852405479479106 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/2_mean",
            "value": 2.818987463999975,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.818892138000003 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/2_median",
            "value": 2.818140900000117,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.818035300000013 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/2_stddev",
            "value": 0.00322344462878061,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0032894661369245563 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/2_cv",
            "value": 0.0011434760423541348,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0011669357945914259 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/8_mean",
            "value": 4.533318899999842,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 4.5329170032257915 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/8_median",
            "value": 4.533265919354607,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 4.532133790322535 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/8_stddev",
            "value": 0.0023767115227281872,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.002527868147763696 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/8_cv",
            "value": 0.0005242762698049481,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0005576691887287527 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/32_mean",
            "value": 11.038855711999988,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 11.03817401599997 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/32_median",
            "value": 11.046643199999835,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 11.04563048000003 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/32_stddev",
            "value": 0.015998063784121153,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.01590402187267885 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/32_cv",
            "value": 0.0014492501941782037,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0014408199988173562 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOn_mean",
            "value": 2.5337462522521808,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5336541081081005 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOn_median",
            "value": 2.5322710270269417,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5320443873873697 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOn_stddev",
            "value": 0.003130141625501585,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0031159984776937166 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOn_cv",
            "value": 0.0012353808605416914,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.001229843674289249 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOff_mean",
            "value": 2.516739076785614,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5165957196428477 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOff_median",
            "value": 2.5187146428576415,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5186975178571442 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOff_stddev",
            "value": 0.003918939926895642,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0038424256510533722 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOff_cv",
            "value": 0.0015571498702602582,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.001526834692224099 ms\nthreads: 1"
          },
          {
            "name": "BM_Execute_ConstFold_mean",
            "value": 13.105463774362349,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.104854292850368 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ConstFold_median",
            "value": 12.846980156562987,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.846455071388794 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ConstFold_stddev",
            "value": 0.4389375894741362,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.43921422127774645 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ConstFold_cv",
            "value": 0.03349271700958884,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.033515383800746965 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/0_mean",
            "value": 13.556377576599592,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.55585218353209 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/0_median",
            "value": 13.399109039675869,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.398634979266646 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/0_stddev",
            "value": 0.23179375582081785,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.23156864155879392 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/0_cv",
            "value": 0.017098502495307432,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.017082558766767012 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/1_mean",
            "value": 14.012799347978694,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.012364957566225 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/1_median",
            "value": 14.00882269565231,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.00830005077402 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/1_stddev",
            "value": 0.007591236566344495,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.007880982379892622 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/1_cv",
            "value": 0.0005417359071397471,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0005624305678419507 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/10_mean",
            "value": 12.784623549731219,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.770373607596657 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/10_median",
            "value": 12.760288385791675,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.759677717369467 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/10_stddev",
            "value": 0.06268873255158999,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.03366741910092831 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/10_cv",
            "value": 0.0049034476695958675,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.002636369156882045 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/100_mean",
            "value": 12.960460683929577,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.959440126509486 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/100_median",
            "value": 12.769883391367296,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.76974040138126 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/100_stddev",
            "value": 0.43087062376460494,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.431007765969301 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/100_cv",
            "value": 0.03324500835829595,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.033258208823978674 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/1000_mean",
            "value": 12.756346388874261,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.755657730811068 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/1000_median",
            "value": 12.755183520525597,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.754306470193582 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/1000_stddev",
            "value": 0.0093558343951013,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.009207207096192 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/1000_cv",
            "value": 0.0007334258658310819,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0007218135897416056 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfThenElse_mean",
            "value": 13.361474790551682,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.36116594654068 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfThenElse_median",
            "value": 13.066189654869357,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.066041095927542 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfThenElse_stddev",
            "value": 0.41235494762192104,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.4121268030286765 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfThenElse_cv",
            "value": 0.0308614845356375,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.030845122699443728 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/2_mean",
            "value": 13.782822496322018,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.782185755080363 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/2_median",
            "value": 13.688208770690864,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.687326496316183 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/2_stddev",
            "value": 0.17745539591934345,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.1776462574030959 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/2_cv",
            "value": 0.012875112914404716,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.012889556167650135 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/8_mean",
            "value": 13.87073059554803,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.8700197142431 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/8_median",
            "value": 13.694614253963612,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.693894672104202 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/8_stddev",
            "value": 0.40772939718812584,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.4080371025809878 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/8_cv",
            "value": 0.029394947467222186,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.029418638977273783 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/32_mean",
            "value": 13.692610057551686,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.691984669872658 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/32_median",
            "value": 13.69281905846976,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.6922783498363 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/32_stddev",
            "value": 0.011449612453298494,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.011610772779773482 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/32_cv",
            "value": 0.0008361891856391437,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.000847997792848936 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfBlock_mean",
            "value": 11.201933226153772,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 11.20103719447726 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfBlock_median",
            "value": 11.203591368901137,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 11.202715543352708 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfBlock_stddev",
            "value": 0.00570704339854612,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.006030716009052511 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfBlock_cv",
            "value": 0.000509469507033087,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.000538406926460881 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_RefNode_mean",
            "value": 11.204869639846537,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 11.204228618025425 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_RefNode_median",
            "value": 11.204815202223356,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 11.203592315304729 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_RefNode_stddev",
            "value": 0.008051762943568625,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.007987532989158604 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_RefNode_cv",
            "value": 0.0007185949682926345,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0007129034279350761 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FunctionCall_Builtin_mean",
            "value": 13.090696753684403,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.090063306481184 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FunctionCall_Builtin_median",
            "value": 13.070708527643182,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.06934762996384 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FunctionCall_Builtin_stddev",
            "value": 0.06234808073820488,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.06221864812245731 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FunctionCall_Builtin_cv",
            "value": 0.004762777865177947,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.004753120490383836 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/16_mean",
            "value": 17.122417242548256,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 17.121471681010764 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/16_median",
            "value": 17.12441632656346,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 17.123120917669308 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/16_stddev",
            "value": 0.0059464076597437725,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.005903868912428717 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/16_cv",
            "value": 0.000347287861025094,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00034482251423378717 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/256_mean",
            "value": 31.952819488012153,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 31.9514604006321 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/256_median",
            "value": 31.447528852991418,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 31.446309882535893 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/256_stddev",
            "value": 1.101450830631041,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1.1010379952022968 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/256_cv",
            "value": 0.03447116242885158,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.03445970798819935 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/4096_mean",
            "value": 357.5722926099355,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 357.555938262287 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/4096_median",
            "value": 357.6112113384594,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 357.6084749838258 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/4096_stddev",
            "value": 0.5883164719721405,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.5873415522266022 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/4096_cv",
            "value": 0.001645307771690008,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0016426564052636563 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/16_mean",
            "value": 21.74196264504358,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 21.74082671284556 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/16_median",
            "value": 21.706301795051047,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 21.705506852682756 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/16_stddev",
            "value": 0.09664010970736482,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0961668529678354 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/16_cv",
            "value": 0.00444486596196942,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.004423330089421818 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/256_mean",
            "value": 119.17661443088232,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 119.16988869421718 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/256_median",
            "value": 119.16194076630173,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 119.16091284710433 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/256_stddev",
            "value": 0.425916170742219,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.42373245301601525 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/256_cv",
            "value": 0.003573823377817411,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0035557006695146574 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/4096_mean",
            "value": 1567.312650480672,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1567.217754469488 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/4096_median",
            "value": 1566.8615858863818,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1566.8450105560883 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/4096_stddev",
            "value": 2.690858323570502,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2.7059071481377623 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/4096_cv",
            "value": 0.0017168612291524952,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0017265674412001077 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/16_mean",
            "value": 18.881788622306555,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 18.880752069852544 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/16_median",
            "value": 18.482955373834507,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 18.481266005230758 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/16_stddev",
            "value": 0.9665166292598523,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.9663216985512346 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/16_cv",
            "value": 0.051187768732779346,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.05118025463054456 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/256_mean",
            "value": 221.4659793422532,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 221.45382832720094 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/256_median",
            "value": 221.43968736398188,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 221.43774189718405 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/256_stddev",
            "value": 0.16492611129196036,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.167038880127865 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/256_cv",
            "value": 0.0007447017902333604,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0007542830999564519 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/4096_mean",
            "value": 3808.976692978079,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3808.831124115421 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/4096_median",
            "value": 3809.3300217758056,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3809.0893032117738 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/4096_stddev",
            "value": 0.7436449241570139,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.7201748647507387 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/4096_cv",
            "value": 0.0001952348318455026,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.000189080282449644 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/256_mean",
            "value": 178.78307787926516,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 178.77593061007803 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/256_median",
            "value": 178.74930255296368,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 178.7320424925862 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/256_stddev",
            "value": 0.15128913379467995,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.15112812249031413 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/256_cv",
            "value": 0.0008462161832611906,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0008453493821824057 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/4096_mean",
            "value": 2568.9366168632837,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2568.81125205218 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/4096_median",
            "value": 2568.008117106291,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2567.9017325665563 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/4096_stddev",
            "value": 2.2934406422010616,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2.3059707025418845 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/4096_cv",
            "value": 0.0008927587497278902,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0008976800847861762 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/256_mean",
            "value": 417.5081122241053,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 417.49098993444693 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/256_median",
            "value": 417.25674107406894,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 417.2260536105535 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/256_stddev",
            "value": 10.881861543591695,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 10.876149447975441 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/256_cv",
            "value": 0.026063832593869823,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.026051219571668307 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/4096_mean",
            "value": 5330.756566575639,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5330.500788595723 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/4096_median",
            "value": 5321.609417651176,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5321.17430618745 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/4096_stddev",
            "value": 32.523862346914065,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 32.482014379361694 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/4096_cv",
            "value": 0.006101171933237738,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.006093614027570346 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/256_mean",
            "value": 15.270374575187802,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 15.270030139236876 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/256_median",
            "value": 15.265408291390742,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 15.264669333794558 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/256_stddev",
            "value": 0.018743149161443585,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.01880488193433569 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/256_cv",
            "value": 0.0012274190832161085,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0012314895100315414 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/4096_mean",
            "value": 15.2703850622202,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 15.269716193248218 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/4096_median",
            "value": 15.268904636150353,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 15.267481570995963 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/4096_stddev",
            "value": 0.017223306545912268,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.01738653458539358 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/4096_cv",
            "value": 0.001127889472055535,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0011386285354197578 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/256_mean",
            "value": 9247.919371555095,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 9247.53481202757 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/256_median",
            "value": 9216.282239167758,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 9216.197808363966 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/256_stddev",
            "value": 84.89544300502854,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 85.08279366100304 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/256_cv",
            "value": 0.00917995060231076,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.009200591875614492 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/4096_mean",
            "value": 196062.74363638536,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 196055.68881118827 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/4096_median",
            "value": 195805.99440563467,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 195803.91188811406 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/4096_stddev",
            "value": 1122.9950403975413,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1125.9515634748059 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/4096_cv",
            "value": 0.005727732967361861,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.005743019089638124 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/256_mean",
            "value": 30.179304643657456,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 30.178368506626214 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/256_median",
            "value": 30.012481609297037,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 30.012205326795605 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/256_stddev",
            "value": 0.2726367304868216,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.272863617296747 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/256_cv",
            "value": 0.009033897026653975,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.009041695452715902 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/4096_mean",
            "value": 344.83879890210443,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 344.81905367439293 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/4096_median",
            "value": 344.21755808769205,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 344.2140978327659 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/4096_stddev",
            "value": 1.1985809448303706,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1.1994696320267941 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/4096_cv",
            "value": 0.003475771718978272,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0034785480072671215 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/256_mean",
            "value": 14.629214631370346,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.628593676063335 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/256_median",
            "value": 14.622739276965158,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.621988508059292 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/256_stddev",
            "value": 0.015182090730735379,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.01530201610997014 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/256_cv",
            "value": 0.0010377926029042917,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0010460346666821925 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/4096_mean",
            "value": 14.627220773025801,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.626737592459046 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/4096_median",
            "value": 14.623135081096361,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.622488111569414 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/4096_stddev",
            "value": 0.009732017661915414,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.009686707028176873 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/4096_cv",
            "value": 0.0006653360753166673,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0006622602591278418 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLen_mean",
            "value": 12.446958375050224,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.44648246304737 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLen_median",
            "value": 12.444300508751402,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.443995204533007 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLen_stddev",
            "value": 0.007451140621741357,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.007721713136141828 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLen_cv",
            "value": 0.0005986314404872662,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0006203932041897772 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/256_mean",
            "value": 12027.418034516902,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12026.960681885505 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/256_median",
            "value": 12049.133592137909,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12048.68163213653 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/256_stddev",
            "value": 191.74899595480295,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 191.8170129755827 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/256_cv",
            "value": 0.015942656637069723,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.01594891827196952 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/4096_mean",
            "value": 182987.33443054202,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 182976.77669519285 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/4096_median",
            "value": 182922.63067807985,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 182908.39302172072 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/4096_stddev",
            "value": 846.8768401319014,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 850.772192210817 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/4096_cv",
            "value": 0.004628062607542695,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.004649618424681586 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/256_mean",
            "value": 1003.4300690095206,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1003.3880203060766 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/256_median",
            "value": 998.3553859675319,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 998.3469646571745 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/256_stddev",
            "value": 12.161034681425868,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.159945030966965 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/256_cv",
            "value": 0.012119464083261873,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.012118885999114936 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/4096_mean",
            "value": 25018.7472431072,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 25017.39575725045 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/4096_median",
            "value": 24996.490959542272,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 24995.57697815999 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/4096_stddev",
            "value": 112.25749921515838,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 111.3337725337049 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/4096_cv",
            "value": 0.004486935261959846,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0044502542796221525 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/256_mean",
            "value": 823.853086678211,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 823.8216741215608 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/256_median",
            "value": 821.7117567101802,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 821.6774180232278 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/256_stddev",
            "value": 14.034988736634592,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.03104576945637 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/256_cv",
            "value": 0.017035790681107832,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.017031654070545837 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/4096_mean",
            "value": 18978.317990387666,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 18977.46980838267 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/4096_median",
            "value": 18984.78461643222,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 18984.00798970901 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/4096_stddev",
            "value": 79.5960524495894,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 80.05739447497731 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/4096_cv",
            "value": 0.004194051996067514,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.004218549431685282 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/256_mean",
            "value": 44.499536728290465,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 44.49718852802299 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/256_median",
            "value": 43.851536779433964,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 43.851079613936214 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/256_stddev",
            "value": 1.449311869578959,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1.449935972602605 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/256_cv",
            "value": 0.03256914512230333,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.03258488953048076 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/4096_mean",
            "value": 425.11057646760025,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 425.0965932447367 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/4096_median",
            "value": 425.85137614387315,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 425.8472792092318 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/4096_stddev",
            "value": 3.561057427974906,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3.5643478043038557 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/4096_cv",
            "value": 0.008376779184289035,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.008384795034694125 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/256_mean",
            "value": 5046.334218837381,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5046.194134738621 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/256_median",
            "value": 5046.163974662273,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5045.928931078656 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/256_stddev",
            "value": 10.993979515965643,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 10.986562333465132 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/256_cv",
            "value": 0.0021786070916441466,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.002177197713784392 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/4096_mean",
            "value": 79460.2092413014,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 79456.45681688531 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/4096_median",
            "value": 79698.04706217205,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 79692.36081004066 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/4096_stddev",
            "value": 575.6970093736389,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 576.9431734296973 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/4096_cv",
            "value": 0.007245098079535464,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00726112384748436 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/256_mean",
            "value": 736.9122761576698,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 736.8731952440007 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/256_median",
            "value": 737.1300398264464,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 737.0704529361699 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/256_stddev",
            "value": 0.8478671732719822,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.8409942129015248 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/256_cv",
            "value": 0.001150567307268705,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0011413011333965628 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/4096_mean",
            "value": 11593.724627237678,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 11593.36018058307 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/4096_median",
            "value": 11592.891401592331,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 11592.28996852243 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/4096_stddev",
            "value": 3.2284141579841665,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3.433880512885538 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/4096_cv",
            "value": 0.00027846220794303697,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00029619372290672996 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/256_mean",
            "value": 229.56962385170323,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 229.55896301517336 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/256_median",
            "value": 230.42865641716872,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 230.41395729989398 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/256_stddev",
            "value": 4.674350350913582,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 4.677242858382835 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/256_cv",
            "value": 0.020361362590083372,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.020374908463381056 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/4096_mean",
            "value": 33659.67493261266,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 33658.85077653689 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/4096_median",
            "value": 35931.22808368155,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 35930.873828775555 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/4096_stddev",
            "value": 5083.702054122488,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5083.42023196342 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/4096_cv",
            "value": 0.15103241681032753,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.15102774202579133 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/256_mean",
            "value": 100.51985196602244,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 100.515272111227 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/256_median",
            "value": 100.48870576270723,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 100.48760700574434 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/256_stddev",
            "value": 0.11328068124300472,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.11540248952099574 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/256_cv",
            "value": 0.0011269483492802564,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0011481090096766091 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/4096_mean",
            "value": 1298.1978827307996,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1298.1669161655986 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/4096_median",
            "value": 1297.4230557369965,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1297.3486319219776 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/4096_stddev",
            "value": 1.4466233450945354,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1.4589686868688143 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/4096_cv",
            "value": 0.001114331924537974,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0011238683321079979 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/256_mean",
            "value": 1394.8981166267497,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1394.788099351317 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/256_median",
            "value": 1394.8153965639349,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1394.7485101786183 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/256_stddev",
            "value": 0.6264998319547814,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.582093963264388 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/256_cv",
            "value": 0.0004491366247377491,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00041733505149284413 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/4096_mean",
            "value": 22087.54897425871,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 22086.804657425942 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/4096_median",
            "value": 22085.094891087585,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 22083.767049504997 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/4096_stddev",
            "value": 13.997882700880421,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.468849714470016 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/4096_cv",
            "value": 0.0006337454063913496,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.000655090219653179 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/256_mean",
            "value": 1854.556037628383,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1854.499132938326 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/256_median",
            "value": 1846.4837333866599,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1846.3937330520996 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/256_stddev",
            "value": 18.27684133543216,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 18.29535538100852 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/256_cv",
            "value": 0.00985510330483445,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.009865389018554454 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/4096_mean",
            "value": 29387.466968040837,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 29386.35595037844 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/4096_median",
            "value": 29375.811396128887,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 29375.539529015823 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/4096_stddev",
            "value": 42.521124080547985,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 42.221651077377146 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/4096_cv",
            "value": 0.00144691354742449,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.001436777365273608 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/256_mean",
            "value": 2031.3453639183288,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2031.2656250450777 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/256_median",
            "value": 2030.5448243523265,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2030.4639039169608 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/256_stddev",
            "value": 3.978312073538009,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3.964523598309597 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/256_cv",
            "value": 0.0019584616895790244,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0019517504502748705 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/4096_mean",
            "value": 32296.238235296933,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 32295.319337656 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/4096_median",
            "value": 32280.197197779613,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 32279.046317739896 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/4096_stddev",
            "value": 86.53845258135712,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 86.6875255662313 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/4096_cv",
            "value": 0.0026795211241282656,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0026842132960473494 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/256_mean",
            "value": 2072.3395160339,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2072.2446533836173 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/256_median",
            "value": 2069.530815255182,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2069.4595319500045 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/256_stddev",
            "value": 13.415981978849914,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.401673023690481 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/256_cv",
            "value": 0.006473833980894108,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0064672252872303776 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/4096_mean",
            "value": 33485.55079625426,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 33484.30761124154 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/4096_median",
            "value": 33000.95725995163,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 33000.624121780296 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/4096_stddev",
            "value": 1024.6143663879288,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1024.2772235596706 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/4096_cv",
            "value": 0.030598701291260928,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.030589768659746594 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/256_mean",
            "value": 454.102527137986,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 454.08757919552227 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/256_median",
            "value": 451.50270593332255,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 451.4818961564013 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/256_stddev",
            "value": 5.256677331613071,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5.260561181128613 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/256_cv",
            "value": 0.011575970221402776,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.011584904371197313 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/4096_mean",
            "value": 7130.085563182829,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 7129.806687007348 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/4096_median",
            "value": 7126.396109839033,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 7126.319196542067 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/4096_stddev",
            "value": 9.522587498276534,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 9.742077228347139 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/4096_cv",
            "value": 0.0013355502418439013,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0013663872887465704 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/256_mean",
            "value": 452.38338268229825,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 452.3583914536205 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/256_median",
            "value": 451.0279485590214,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 450.9827819694427 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/256_stddev",
            "value": 2.9120733935907483,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2.919439983797475 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/256_cv",
            "value": 0.006437180287932574,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00645382077342717 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/4096_mean",
            "value": 7090.009809291444,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 7089.64503630751 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/4096_median",
            "value": 7068.68574587377,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 7068.401919646351 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/4096_stddev",
            "value": 47.93904900818214,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 47.9424508563108 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/4096_cv",
            "value": 0.0067614926209774925,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.006762320343372311 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/256_mean",
            "value": 720.49961344826,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 720.4405668035149 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/256_median",
            "value": 718.13259694457,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 718.079106102093 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/256_stddev",
            "value": 5.89954001864196,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5.927809117849505 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/256_cv",
            "value": 0.008188123780396189,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00822803349921048 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/4096_mean",
            "value": 11329.272902674888,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 11328.560581267666 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/4096_median",
            "value": 11301.118899339319,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 11301.016200594535 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/4096_stddev",
            "value": 49.84331597042159,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 50.27623374819183 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/4096_cv",
            "value": 0.004399515873490291,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.004438007228502273 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_Scalar/4096_mean",
            "value": 1847.0943189245445,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1847.0052208483692 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_Scalar/4096_median",
            "value": 1868.6582931838507,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1868.5848118324564 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_Scalar/4096_stddev",
            "value": 50.18759268253878,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 50.18989567872245 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_Scalar/4096_cv",
            "value": 0.027171104457600247,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.027173662051517732 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_LL/4096_mean",
            "value": 2117.322295010676,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2117.232035624595 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_LL/4096_median",
            "value": 2099.459025471381,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2099.2889117652 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_LL/4096_stddev",
            "value": 44.22685112723386,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 44.22904024652398 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_LL/4096_cv",
            "value": 0.020888105335428328,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.020890029766376633 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_Scalar/4096_mean",
            "value": 1806.2296294835546,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1806.1746149495652 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_Scalar/4096_median",
            "value": 1755.1999434425657,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1755.092083283966 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_Scalar/4096_stddev",
            "value": 75.55361871229671,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 75.57190458709056 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_Scalar/4096_cv",
            "value": 0.04182946480282208,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.04184086298278575 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_LL/4096_mean",
            "value": 2073.376609292101,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2073.2327020633957 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_LL/4096_median",
            "value": 2069.5380100863886,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2069.518409973421 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_LL/4096_stddev",
            "value": 14.06102558000976,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.054963716896584 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_LL/4096_cv",
            "value": 0.0067817035829349495,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00677925044444278 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_Scalar/4096_mean",
            "value": 2510.2431995428865,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2510.054403654877 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_Scalar/4096_median",
            "value": 2499.356459586076,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2499.1760123497224 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_Scalar/4096_stddev",
            "value": 27.801503521899633,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 27.811222514895015 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_Scalar/4096_cv",
            "value": 0.011075223120597345,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.011079928177811298 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_LL/4096_mean",
            "value": 2717.405239162222,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2717.230351680492 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_LL/4096_median",
            "value": 2721.084442279926,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2721.0628348759305 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_LL/4096_stddev",
            "value": 14.73324046565681,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.62389679829396 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_LL/4096_cv",
            "value": 0.00542180468828384,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.005381912795597068 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_Scalar/4096_mean",
            "value": 8706.421801396904,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8706.053080838203 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_Scalar/4096_median",
            "value": 8738.184344470135,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8737.537320982472 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_Scalar/4096_stddev",
            "value": 59.08323470410213,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 59.09785284194866 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_Scalar/4096_cv",
            "value": 0.00678616727420931,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.006788133760868229 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_LL/4096_mean",
            "value": 8862.105948228855,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8861.608479185898 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_LL/4096_median",
            "value": 8941.60061434263,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8941.482225706783 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_LL/4096_stddev",
            "value": 112.02333166053617,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 111.93814480599012 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_LL/4096_cv",
            "value": 0.012640712299645291,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.012631808894390887 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_Scalar/4096_mean",
            "value": 8796.311539554083,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8795.9770786657 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_Scalar/4096_median",
            "value": 8834.34845855326,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8834.073427786645 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_Scalar/4096_stddev",
            "value": 61.39717822933801,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 61.52939643895409 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_Scalar/4096_cv",
            "value": 0.006979877639992098,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.006995174713243767 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_LL/4096_mean",
            "value": 8780.044566589557,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8779.626585797403 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_LL/4096_median",
            "value": 8747.66906747367,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8747.26203563259 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_LL/4096_stddev",
            "value": 49.55960727529353,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 49.86557317171226 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_LL/4096_cv",
            "value": 0.005644573543952297,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0056796918051593035 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax_Scalar/4096_mean",
            "value": 3416.240469180289,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3416.042902934844 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax_Scalar/4096_median",
            "value": 3400.585620666566,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3400.2893555893206 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax_Scalar/4096_stddev",
            "value": 152.4558584774437,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 152.39539616999082 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax_Scalar/4096_cv",
            "value": 0.044626793650162686,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.04461167511656909 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin_Scalar/4096_mean",
            "value": 3615.485562053452,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3615.378197825482 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin_Scalar/4096_median",
            "value": 3645.3246983895697,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3645.119058190708 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin_Scalar/4096_stddev",
            "value": 58.4839879708624,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 58.5135711166251 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin_Scalar/4096_cv",
            "value": 0.016175970548654557,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.016184633505788935 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_Scalar/4096_mean",
            "value": 1839.5535889345551,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1839.4684448158478 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_Scalar/4096_median",
            "value": 1866.639135911652,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1866.4913815682964 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_Scalar/4096_stddev",
            "value": 87.54362179833392,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 87.53877658279839 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_Scalar/4096_cv",
            "value": 0.04758960126246608,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.04758917002871558 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_LL/4096_mean",
            "value": 2080.3714787975155,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2080.255598988262 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_LL/4096_median",
            "value": 2075.083282030273,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2074.9921450241204 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_LL/4096_stddev",
            "value": 8.459526531100295,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8.4503291189569 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_LL/4096_cv",
            "value": 0.004066353830225563,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.004062159055390473 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_Scalar/4096_mean",
            "value": 1767.0919020596393,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1766.9984764585613 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_Scalar/4096_median",
            "value": 1761.8726665709244,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1761.7253639121604 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_Scalar/4096_stddev",
            "value": 58.13530824963294,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 58.07796568609585 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_Scalar/4096_cv",
            "value": 0.03289885952274081,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.03286814700740228 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_LL/4096_mean",
            "value": 2090.6746110113295,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2090.5968686414844 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_LL/4096_median",
            "value": 2092.654585576949,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2092.5880760023215 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_LL/4096_stddev",
            "value": 20.017924677983068,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 20.064303757636445 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_LL/4096_cv",
            "value": 0.009574863813120936,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.009597404482230313 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_Scalar/4096_mean",
            "value": 1863.5340595072653,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1863.4709384975954 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_Scalar/4096_median",
            "value": 1866.4917448128745,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1866.4049157023333 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_Scalar/4096_stddev",
            "value": 9.585098919574033,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 9.60860936759114 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_Scalar/4096_cv",
            "value": 0.005143506162752088,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.005156296870042944 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_LL/4096_mean",
            "value": 2098.3941222146664,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2098.2760602711883 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_LL/4096_median",
            "value": 2076.0033002809087,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2075.920472194087 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_LL/4096_stddev",
            "value": 46.734749054927434,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 46.78477991945704 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_LL/4096_cv",
            "value": 0.022271673638507482,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.02229677057527426 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenEqualBitmap/4096_mean",
            "value": 1853.220487255164,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1853.1312539361686 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenEqualBitmap/4096_median",
            "value": 1854.3061553254713,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1854.2437071164945 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenEqualBitmap/4096_stddev",
            "value": 2.5213614970247953,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2.5382297341480102 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenEqualBitmap/4096_cv",
            "value": 0.0013605296910780575,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0013696977635861726 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenNotEqualBitmap/4096_mean",
            "value": 1854.3149456146016,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1854.2553602009543 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenNotEqualBitmap/4096_median",
            "value": 1853.9442457105429,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1853.9244222567 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenNotEqualBitmap/4096_stddev",
            "value": 2.1305592058651763,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2.165533935434708 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenNotEqualBitmap/4096_cv",
            "value": 0.0011489737549189708,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0011678725497657554 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeBitmap/4096_mean",
            "value": 1855.2850368834766,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1855.1871119248754 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeBitmap/4096_median",
            "value": 1855.0266279250543,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1854.8240327971903 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeBitmap/4096_stddev",
            "value": 0.7377604028996456,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.7366829716286531 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeBitmap/4096_cv",
            "value": 0.0003976534000074412,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0003970936230061977 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeEqualBitmap/4096_mean",
            "value": 1855.943446553653,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1855.8739642140329 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeEqualBitmap/4096_median",
            "value": 1855.7839561112992,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1855.637770486522 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeEqualBitmap/4096_stddev",
            "value": 2.441261611247298,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2.4559227292880292 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeEqualBitmap/4096_cv",
            "value": 0.0013153750001275828,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.001323324092392297 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessBitmap/4096_mean",
            "value": 1860.4501222188233,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1860.378722699546 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessBitmap/4096_median",
            "value": 1855.929822004778,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1855.9110354466961 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessBitmap/4096_stddev",
            "value": 11.565606595047026,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 11.574519380597298 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessBitmap/4096_cv",
            "value": 0.006216563646035062,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.006221593076382761 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessEqualBitmap/4096_mean",
            "value": 1830.9209696257253,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1830.8240644677237 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessEqualBitmap/4096_median",
            "value": 1826.6635933570142,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1826.6454406056491 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessEqualBitmap/4096_stddev",
            "value": 9.813799205384086,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 9.819837946408756 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessEqualBitmap/4096_cv",
            "value": 0.005360034304151429,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.005363616382912075 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfEqual/4096_mean",
            "value": 2398.294874056933,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2398.1792975274498 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfEqual/4096_median",
            "value": 2404.1064072154563,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2403.94220574006 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfEqual/4096_stddev",
            "value": 19.266647784395317,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 19.352837357469532 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfEqual/4096_cv",
            "value": 0.008033477448002061,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00806980419580076 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfNotEqual/4096_mean",
            "value": 2510.388200554181,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2510.305000085955 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfNotEqual/4096_median",
            "value": 2507.3286299695787,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2507.302691161873 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfNotEqual/4096_stddev",
            "value": 9.659483406486313,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 9.6074087385571 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfNotEqual/4096_cv",
            "value": 0.0038478046560105456,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0038271878270680786 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLarge/4096_mean",
            "value": 2404.082175496717,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2403.9988627774246 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLarge/4096_median",
            "value": 2402.3442846838143,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2402.320320552494 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLarge/4096_stddev",
            "value": 4.132803748199047,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 4.114593170692665 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLarge/4096_cv",
            "value": 0.0017190775716080305,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0017115620287519314 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLargeEqual/4096_mean",
            "value": 2533.194533655221,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2533.1013006131316 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLargeEqual/4096_median",
            "value": 2504.8560978016385,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2504.8274466156495 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLargeEqual/4096_stddev",
            "value": 76.05213528436997,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 76.0416114920421 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLargeEqual/4096_cv",
            "value": 0.030022224615585326,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.030019175101144352 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLess/4096_mean",
            "value": 2507.7043227770223,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2507.618723358488 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLess/4096_median",
            "value": 2507.3189539906357,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2507.17812497184 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLess/4096_stddev",
            "value": 1.1279452803468755,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1.164401245260008 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLess/4096_cv",
            "value": 0.0004497919751152293,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0004643454104141117 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLessEqual/4096_mean",
            "value": 2570.094803369914,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2570.0020929753764 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLessEqual/4096_median",
            "value": 2610.4891546636527,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2610.3931624695106 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLessEqual/4096_stddev",
            "value": 58.074841409494454,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 58.104401995445734 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLessEqual/4096_cv",
            "value": 0.022596381010282808,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.022608698317508502 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfByBitmap/4096_mean",
            "value": 4146.103735144537,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 4145.950450793206 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfByBitmap/4096_median",
            "value": 4101.01757801122,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 4100.872680170965 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfByBitmap/4096_stddev",
            "value": 62.95242854674668,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 62.97598990637467 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfByBitmap/4096_cv",
            "value": 0.015183515070577969,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.015189759417970386 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FilterByBitmap/4096_mean",
            "value": 1821.3404071808181,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1821.2267415504634 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FilterByBitmap/4096_median",
            "value": 1807.253749206627,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1807.107912947574 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FilterByBitmap/4096_stddev",
            "value": 29.99806996078067,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 29.988308534192374 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FilterByBitmap/4096_cv",
            "value": 0.016470325833935408,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.016465993964410192 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaReuse_mean",
            "value": 12.983168368974495,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.982781942214553 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaReuse_median",
            "value": 12.757292760461029,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.757175825587327 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaReuse_stddev",
            "value": 0.3322089841198163,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.3321784208357236 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaReuse_cv",
            "value": 0.02558766663718901,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.025586074102933128 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaFresh_mean",
            "value": 13.079735067572292,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.079160009365022 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaFresh_median",
            "value": 13.080017481663935,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.079888742171292 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaFresh_stddev",
            "value": 0.008508510342675215,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.008168426404368495 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaFresh_cv",
            "value": 0.0006505109085710606,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0006245375389948349 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaReuse_mean",
            "value": 31.582283437242495,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 31.58084110840837 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaReuse_median",
            "value": 31.496381805127875,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 31.49342358565055 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaReuse_stddev",
            "value": 0.23376174555566806,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.23350695590056292 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaReuse_cv",
            "value": 0.007401673347026303,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.007393943533644261 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaFresh_mean",
            "value": 31.54723055476571,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 31.545697053581534 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaFresh_median",
            "value": 31.55111869926906,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 31.549910938195545 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaFresh_stddev",
            "value": 0.01930830227574299,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.02020523658837257 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaFresh_cv",
            "value": 0.0006120442883955835,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0006405068987397307 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/2_mean",
            "value": 33.94054138890337,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 33.93835715459523 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/2_median",
            "value": 33.939885963606756,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 33.93953876572677 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/2_stddev",
            "value": 0.027952422793414495,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.027970162968769297 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/2_cv",
            "value": 0.0008235703276834986,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0008241460493022761 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/8_mean",
            "value": 102.33139840801125,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 102.32468412632309 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/8_median",
            "value": 102.11638122370776,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 102.11124438318787 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/8_stddev",
            "value": 0.3808528941721974,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.3782109448213171 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/8_cv",
            "value": 0.0037217598908760876,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.003696184826277144 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/32_mean",
            "value": 553.914548738302,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 553.8883846481059 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/32_median",
            "value": 554.6057228341439,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 554.578573406778 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/32_stddev",
            "value": 5.1445513577387185,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5.129804489316816 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/32_cv",
            "value": 0.009287626348607197,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.009261440809190943 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/2_mean",
            "value": 24.894049057575554,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 24.892854286133293 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/2_median",
            "value": 24.892895507853915,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 24.892583631866533 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/2_stddev",
            "value": 0.012970224361742189,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.013580278200981153 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/2_cv",
            "value": 0.0005210170644295085,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00054554925862986 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/8_mean",
            "value": 95.10888138395451,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 95.10386964551552 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/8_median",
            "value": 92.99722953879032,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 92.99637767676046 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/8_stddev",
            "value": 2.9394680363654007,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2.9370340932374033 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/8_cv",
            "value": 0.030906346427299145,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.03088238264315352 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/32_mean",
            "value": 524.4690230610594,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 524.453033455792 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/32_median",
            "value": 523.66411845535,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 523.6268992400336 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/32_stddev",
            "value": 3.1547101819056733,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3.158505727210332 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/32_cv",
            "value": 0.006015055309641038,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0060224758476424625 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_i64_mean",
            "value": 0.3519835692055613,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.35196476790261205 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_i64_median",
            "value": 0.3519083933546986,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.3518954393998114 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_i64_stddev",
            "value": 0.00038003477820692594,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00038881449295005894 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_i64_cv",
            "value": 0.0010796946546814022,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0011046971981515013 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_f64_mean",
            "value": 0.31163739278176283,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.3116256842298972 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_f64_median",
            "value": 0.31150096436128705,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.31148441207978095 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_f64_stddev",
            "value": 0.00026092533929622313,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00026140294855963225 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_f64_cv",
            "value": 0.0008372722444092164,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0008388363404820834 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/16_mean",
            "value": 2.179466100894294,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2.179371926827618 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/16_median",
            "value": 2.178836172926569,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2.1786720445860683 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/16_stddev",
            "value": 0.0016448383219767916,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.001659552842122947 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/16_cv",
            "value": 0.0007546978231512157,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0007614821599260752 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/256_mean",
            "value": 20.904634369600476,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 20.90371193207222 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/256_median",
            "value": 20.878421847578355,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 20.87825320453873 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/256_stddev",
            "value": 0.053635118628251474,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.052986562525367556 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/256_cv",
            "value": 0.002565704698774722,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0025347920358618765 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/4096_mean",
            "value": 330.3063286230927,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 330.29426470362813 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/4096_median",
            "value": 329.45802829872287,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 329.4398125792344 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/4096_stddev",
            "value": 1.7259949465774802,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1.7258758198504172 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/4096_cv",
            "value": 0.005225437107948923,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.005225267297326641 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "598855847@qq.com",
            "name": "Weiqiang Wu",
            "username": "viktorika"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "8b66d1ba287e962cc11c4e5e5ffdfd6520d1703d",
          "message": "Merge pull request #222 from viktorika/dev\n\nadd fp_math_mode option",
          "timestamp": "2026-04-24T11:29:25+08:00",
          "tree_id": "cfeac16a1175aa508d66d02168fe36fa639738ed",
          "url": "https://github.com/viktorika/jitfusion/commit/8b66d1ba287e962cc11c4e5e5ffdfd6520d1703d"
        },
        "date": 1777001704272,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_Compile_MinimalExpr_mean",
            "value": 2.529734039999932,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5295752090909085 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_MinimalExpr_median",
            "value": 2.5136590454549568,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5135610000000006 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_MinimalExpr_stddev",
            "value": 0.03030946366745902,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.03031247358431226 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_MinimalExpr_cv",
            "value": 0.011981284667956572,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.011983226857764036 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/10_mean",
            "value": 2.529280874999935,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5291819928571444 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/10_median",
            "value": 2.514742107143166,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5147552053571434 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/10_stddev",
            "value": 0.02851008716190178,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.028555307768127806 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/10_cv",
            "value": 0.011272013102104572,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.011290333336538467 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/100_mean",
            "value": 2.56101231711725,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.560871203603603 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/100_median",
            "value": 2.557166864864673,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5570879369369357 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/100_stddev",
            "value": 0.009610275912976354,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.009330234396582317 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/100_cv",
            "value": 0.0037525301415941496,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0036433829172872936 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/1000_mean",
            "value": 2.996933676595677,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.9968818106382966 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/1000_median",
            "value": 3.000782180850926,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 3.000664446808515 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/1000_stddev",
            "value": 0.008251044902027601,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.00826772594371431 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/1000_cv",
            "value": 0.0027531623293713505,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0027587761100106222 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/10_mean",
            "value": 2.5514151872727617,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.551360136363634 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/10_median",
            "value": 2.550605218181607,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.550612699999996 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/10_stddev",
            "value": 0.00902321622153068,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.009032943800143198 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/10_cv",
            "value": 0.003536553465128388,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0035404424767008953 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/50_mean",
            "value": 2.6946926730768337,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.694622425000004 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/50_median",
            "value": 2.6966501153844127,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.6966629230769263 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/50_stddev",
            "value": 0.011966509191868372,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.011951918390103487 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/50_cv",
            "value": 0.004440769558409369,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.004435470542817689 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/200_mean",
            "value": 3.219952491953932,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 3.219859183908042 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/200_median",
            "value": 3.2215240000000294,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 3.2214291034482736 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/200_stddev",
            "value": 0.004812962555919316,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.004827057024891003 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/200_cv",
            "value": 0.0014947309216350314,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0014991515930309273 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/10_mean",
            "value": 2.54049673636349,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5404495127272755 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/10_median",
            "value": 2.5442672272726505,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5442756909091004 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/10_stddev",
            "value": 0.006267679638587024,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.006335502933150798 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/10_cv",
            "value": 0.002467107927703417,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0024938511477637587 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/100_mean",
            "value": 2.702992215384589,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.7028552249999973 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/100_median",
            "value": 2.6960123365383355,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.695281682692311 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/100_stddev",
            "value": 0.012525139884316156,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.012631211119707548 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/100_cv",
            "value": 0.004633805385389927,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0046732843856657404 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ListOps_mean",
            "value": 2.7359903000002284,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.7354322745097996 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ListOps_median",
            "value": 2.7373866764708277,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.736141019607836 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ListOps_stddev",
            "value": 0.007090826818938219,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.006655913483474304 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ListOps_cv",
            "value": 0.0025916856572691894,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.002433221814883745 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/2_mean",
            "value": 2.8333740767677247,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.8332061676767677 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/2_median",
            "value": 2.8331299090909434,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.8331638484848383 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/2_stddev",
            "value": 0.001510219258093171,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0017200312644782625 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/2_cv",
            "value": 0.0005330108969642331,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0006070971057812889 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/8_mean",
            "value": 4.560252081966878,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 4.559860396721329 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/8_median",
            "value": 4.558730377049018,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 4.557975491803325 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/8_stddev",
            "value": 0.005508751104042924,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.005385521443328987 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/8_cv",
            "value": 0.0012079926734372432,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0011810715624542658 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/32_mean",
            "value": 11.129480727999182,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 11.128974463999981 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/32_median",
            "value": 11.127130879999642,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 11.12590267999991 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/32_stddev",
            "value": 0.014716564444035712,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0145556657595172 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/32_cv",
            "value": 0.0013223046792302056,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0013079071936593872 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOn_mean",
            "value": 2.542717741818334,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.542534025454541 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOn_median",
            "value": 2.5434320727278896,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.543439318181807 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOn_stddev",
            "value": 0.004180993411617125,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.004256634790116917 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOn_cv",
            "value": 0.0016443010338328928,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0016741702362688886 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOff_mean",
            "value": 2.519064740540711,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.518954394594597 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOff_median",
            "value": 2.52052702702721,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5205394414414357 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOff_stddev",
            "value": 0.0044823631206289665,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.004557054523660243 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOff_cv",
            "value": 0.0017793759122152763,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0018091056088348355 ms\nthreads: 1"
          },
          {
            "name": "BM_Execute_ConstFold_mean",
            "value": 13.687329162076654,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.687060435218305 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ConstFold_median",
            "value": 13.685701691282466,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.685731150667488 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ConstFold_stddev",
            "value": 0.010080708826708867,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.009706223006665673 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ConstFold_cv",
            "value": 0.0007364993350667263,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0007091532219504563 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/0_mean",
            "value": 14.33433455090423,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.334246075439196 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/0_median",
            "value": 14.303711155221839,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.30320567594875 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/0_stddev",
            "value": 0.07130351482005035,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.07134436897915947 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/0_cv",
            "value": 0.004974316356775168,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.004977197168493112 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/1_mean",
            "value": 14.780004130809521,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.779376837540577 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/1_median",
            "value": 14.620304603175061,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.61983183891194 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/1_stddev",
            "value": 0.3558196613961981,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.35593340125835254 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/1_cv",
            "value": 0.024074395260450403,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.02408311291950136 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/10_mean",
            "value": 13.684833190878644,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.683856773474139 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/10_median",
            "value": 13.684786588206169,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.68428111490947 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/10_stddev",
            "value": 0.008206642351192336,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.007795312413408471 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/10_cv",
            "value": 0.0005996888845281877,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0005696721722869474 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/100_mean",
            "value": 13.681189133680562,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.680578447794868 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/100_median",
            "value": 13.67647495610125,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.675300793528672 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/100_stddev",
            "value": 0.009222692439605062,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.009475036932140888 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/100_cv",
            "value": 0.0006741148265321833,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.000692590373155467 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/1000_mean",
            "value": 13.686114973272765,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.68493223684161 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/1000_median",
            "value": 13.682575697627815,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.681048971944904 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/1000_stddev",
            "value": 0.010822312626998808,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.010291699870077484 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/1000_cv",
            "value": 0.000790751257616453,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.000752046096536079 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfThenElse_mean",
            "value": 13.857125372685195,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.856593687943363 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfThenElse_median",
            "value": 13.698061266075811,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.696654466157321 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfThenElse_stddev",
            "value": 0.2751824688031238,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.2748350896843057 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfThenElse_cv",
            "value": 0.01985855373334186,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.01983424612669707 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/2_mean",
            "value": 14.309594594781771,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.308869096251598 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/2_median",
            "value": 14.303956293313732,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.30368995254904 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/2_stddev",
            "value": 0.012995079917506392,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.012875379944477497 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/2_cv",
            "value": 0.0009081375318798524,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0008998181378184789 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/8_mean",
            "value": 14.309974479486693,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.309063855139183 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/8_median",
            "value": 14.305949253707832,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.30598692765253 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/8_stddev",
            "value": 0.009267630564451936,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.008391149166345552 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/8_cv",
            "value": 0.0006476343181280343,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0005864219526375111 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/32_mean",
            "value": 14.62564880295086,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.624862516976606 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/32_median",
            "value": 14.30908646006716,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.307998385146664 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/32_stddev",
            "value": 0.4466562710619474,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.4472396989364712 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/32_cv",
            "value": 0.030539244930579107,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.030580779711078535 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfBlock_mean",
            "value": 10.581602361302897,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 10.580701754319833 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfBlock_median",
            "value": 10.582242377070216,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 10.580571368160875 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfBlock_stddev",
            "value": 0.004798754240906052,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.004434950743163766 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfBlock_cv",
            "value": 0.00045349977036135647,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00041915468804827507 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_RefNode_mean",
            "value": 10.574100520226668,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 10.573580273709801 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_RefNode_median",
            "value": 10.572698016057362,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 10.572691556701336 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_RefNode_stddev",
            "value": 0.0051949353995434724,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.005074767743308719 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_RefNode_cv",
            "value": 0.0004912886339227002,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0004799479090281884 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FunctionCall_Builtin_mean",
            "value": 13.704655969837273,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.703954229343093 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FunctionCall_Builtin_median",
            "value": 13.685142033802412,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.683994541616533 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FunctionCall_Builtin_stddev",
            "value": 0.03684433594507425,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.03725327774368616 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FunctionCall_Builtin_cv",
            "value": 0.002688453911295939,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0027184327326428845 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/16_mean",
            "value": 18.379553415870003,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 18.378311864031712 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/16_median",
            "value": 18.381404070740068,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 18.378986883780243 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/16_stddev",
            "value": 0.010267933715048708,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.009382435269502883 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/16_cv",
            "value": 0.0005586606748661674,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0005105167078955328 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/256_mean",
            "value": 33.66151983483151,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 33.659845398729075 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/256_median",
            "value": 33.65032207494427,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 33.645220625162835 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/256_stddev",
            "value": 0.03236184375438697,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.03225834817121989 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/256_cv",
            "value": 0.0009613898574151815,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.000958362933313945 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/4096_mean",
            "value": 357.52118998925636,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 357.51134549571714 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/4096_median",
            "value": 357.1918648537091,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 357.19206503833186 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/4096_stddev",
            "value": 1.3652366344362332,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1.3773984813332398 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/4096_cv",
            "value": 0.0038186173929362316,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0038527406156114302 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/16_mean",
            "value": 26.437236896407835,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 26.43735874444788 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/16_median",
            "value": 26.43454754563566,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 26.43453883273993 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/16_stddev",
            "value": 0.015570281381155283,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.01563273357696863 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/16_cv",
            "value": 0.0005889526746749732,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0005913122308502797 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/256_mean",
            "value": 179.78912265101684,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 179.78875166560573 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/256_median",
            "value": 179.75576407493008,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 179.75584063969416 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/256_stddev",
            "value": 0.12467665384788092,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.12441480850781102 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/256_cv",
            "value": 0.0006934604942140296,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0006920055195622789 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/4096_mean",
            "value": 2570.6232145804797,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2570.590307853099 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/4096_median",
            "value": 2570.936018262335,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2570.931581070429 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/4096_stddev",
            "value": 1.8229554768954308,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1.7830516879256582 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/4096_cv",
            "value": 0.0007091492314220516,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.000693635108822465 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/16_mean",
            "value": 19.32580994307069,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 19.325538028086303 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/16_median",
            "value": 19.315673172931298,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 19.314869076552505 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/16_stddev",
            "value": 0.026607193678061098,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.026706503758208083 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/16_cv",
            "value": 0.001376769913211382,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0013819280849720632 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/256_mean",
            "value": 222.11504269868402,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 222.1098909243276 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/256_median",
            "value": 222.12980132138537,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 222.12376113491078 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/256_stddev",
            "value": 0.0554226238583999,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.05096543540269063 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/256_cv",
            "value": 0.00024952215385783176,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00022946044946757662 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/4096_mean",
            "value": 3809.941269495836,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3809.901137755478 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/4096_median",
            "value": 3810.1809112940427,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3810.062821524754 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/4096_stddev",
            "value": 1.0730464765601386,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1.0601110284480302 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/4096_cv",
            "value": 0.00028164383665214165,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0002782515845207921 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/256_mean",
            "value": 183.49707611464336,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 183.49383120650464 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/256_median",
            "value": 180.65251222853146,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 180.64472839413915 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/256_stddev",
            "value": 4.406133348935544,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 4.405887058120458 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/256_cv",
            "value": 0.024012008486624204,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.024011090886003984 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/4096_mean",
            "value": 2573.7338081568064,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2573.682924764424 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/4096_median",
            "value": 2572.0677635453885,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2572.071536366274 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/4096_stddev",
            "value": 5.457796963727851,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5.400092646007473 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/4096_cv",
            "value": 0.0021205755414296255,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0020981965548463035 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/256_mean",
            "value": 413.39776417539554,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 413.3983271756659 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/256_median",
            "value": 410.1659189845398,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 410.1666238181762 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/256_stddev",
            "value": 10.289381075188675,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 10.289376618468577 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/256_cv",
            "value": 0.024889784045428746,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.024889739367755834 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/4096_mean",
            "value": 7282.9920876699525,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 7282.891491301374 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/4096_median",
            "value": 5223.277292410149,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5223.284129757689 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/4096_stddev",
            "value": 4608.58446648387,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 4608.443073822871 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/4096_cv",
            "value": 0.6327872406021375,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.6327765667423658 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/256_mean",
            "value": 15.883504409508802,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 15.88326267378039 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/256_median",
            "value": 15.882918170843459,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 15.882947070572474 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/256_stddev",
            "value": 0.008198346480358425,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.008301642583527077 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/256_cv",
            "value": 0.0005161547646532217,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0005226660764876209 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/4096_mean",
            "value": 15.877428310098015,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 15.876982644762236 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/4096_median",
            "value": 15.874048636905357,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 15.874051979596981 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/4096_stddev",
            "value": 0.011792191412942321,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.011652252912873995 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/4096_cv",
            "value": 0.0007427016001982206,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0007339085249121963 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/256_mean",
            "value": 9229.627828475086,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 9229.513027835073 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/256_median",
            "value": 9210.175776008313,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 9209.874856899669 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/256_stddev",
            "value": 47.15081701204459,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 47.074097790697856 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/256_cv",
            "value": 0.005108636869037744,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00510038803225351 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/4096_mean",
            "value": 192759.45696200564,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 192752.4097046432 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/4096_median",
            "value": 192671.18846690774,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 192663.8818565399 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/4096_stddev",
            "value": 485.3733681659745,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 486.6387537442496 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/4096_cv",
            "value": 0.0025180262271730994,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.002524683112859299 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/256_mean",
            "value": 30.049517804541125,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 30.049322791768287 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/256_median",
            "value": 29.83780460616186,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 29.837908136723748 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/256_stddev",
            "value": 0.4985035414607095,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.498646311908693 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/256_cv",
            "value": 0.016589402355913177,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.016594261220598695 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/4096_mean",
            "value": 345.26751823737357,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 345.26259858044506 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/4096_median",
            "value": 345.3022094340817,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 345.30330737381945 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/4096_stddev",
            "value": 0.23275536306786168,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.23365729262260543 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/4096_cv",
            "value": 0.0006741304952637941,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0006767524011673801 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/256_mean",
            "value": 15.716112461900048,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 15.715836755973717 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/256_median",
            "value": 15.554148611679613,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 15.5541842623483 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/256_stddev",
            "value": 0.3654364587215726,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.3650012720481201 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/256_cv",
            "value": 0.023252344344537226,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.023225061300625953 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/4096_mean",
            "value": 15.545425139835036,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 15.544930010551676 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/4096_median",
            "value": 15.544135150960676,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 15.543676960937319 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/4096_stddev",
            "value": 0.01036007145251637,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.009559351504719626 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/4096_cv",
            "value": 0.0006664386055270219,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0006149497938061397 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLen_mean",
            "value": 13.371201218835171,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.3702503782938 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLen_median",
            "value": 13.369879024896193,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.36884021965702 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLen_stddev",
            "value": 0.006384551728553342,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.006648877088342378 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLen_cv",
            "value": 0.00047748527780434755,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0004972888988778124 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/256_mean",
            "value": 12015.894981475274,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12015.25947288632 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/256_median",
            "value": 12052.355043788799,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12052.388346244163 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/256_stddev",
            "value": 146.57478114572922,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 145.93299322677893 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/256_cv",
            "value": 0.01219840730729599,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.012145638099292975 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/4096_mean",
            "value": 184267.86391750135,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 184258.23801546666 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/4096_median",
            "value": 180983.9529638607,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 180979.16301546403 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/4096_stddev",
            "value": 5353.279527570761,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5342.818151558902 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/4096_cv",
            "value": 0.029051617649225477,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.028996359723739595 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/256_mean",
            "value": 1048.1877403244077,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1048.1516272080285 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/256_median",
            "value": 1047.231109212474,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1047.1369388747494 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/256_stddev",
            "value": 10.399142460427598,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 10.384095984388079 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/256_cv",
            "value": 0.009921068583772145,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.009907055157704897 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/4096_mean",
            "value": 25083.798259619543,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 25083.114631739554 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/4096_median",
            "value": 25086.10720373224,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 25086.106306629168 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/4096_stddev",
            "value": 33.08191503674072,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 32.55059318111198 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/4096_cv",
            "value": 0.001318855888344339,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0012977093817497156 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/256_mean",
            "value": 862.3989319204275,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 862.380095101306 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/256_median",
            "value": 857.6606916567962,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 857.6217580468104 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/256_stddev",
            "value": 13.33430614088517,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.325124372374242 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/256_cv",
            "value": 0.015461876919526973,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.015451567641770424 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/4096_mean",
            "value": 19808.83306587978,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 19808.48821790538 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/4096_median",
            "value": 19791.40223817243,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 19791.43032094678 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/4096_stddev",
            "value": 69.69996660213069,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 69.65331923304602 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/4096_cv",
            "value": 0.003518630621517385,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0035163369595305445 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/256_mean",
            "value": 50.039005084032325,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 50.03666725818717 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/256_median",
            "value": 46.39355192875683,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 46.39226843820996 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/256_stddev",
            "value": 10.343741670503555,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 10.343384228993596 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/256_cv",
            "value": 0.20671357580217536,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.20671609033475696 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/4096_mean",
            "value": 452.68090757826405,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 452.6625903784423 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/4096_median",
            "value": 422.1797416529632,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 422.1804400918829 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/4096_stddev",
            "value": 42.54197891730475,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 42.53967991572045 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/4096_cv",
            "value": 0.09397785107592518,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.09397657509129645 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/256_mean",
            "value": 5236.182411262611,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5236.088343260393 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/256_median",
            "value": 5234.426144106517,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5234.084601452488 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/256_stddev",
            "value": 4.827372959043098,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 4.838460847668744 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/256_cv",
            "value": 0.000921926048385901,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0009240602011416682 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/4096_mean",
            "value": 83085.10750519563,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 83083.89682586752 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/4096_median",
            "value": 83118.83061408495,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 83119.08395135029 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/4096_stddev",
            "value": 154.40661431268177,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 153.0182256004843 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/4096_cv",
            "value": 0.0018584150511332747,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0018417314479265408 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/256_mean",
            "value": 659.5547773076541,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 659.5357610106191 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/256_median",
            "value": 659.178936267964,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 659.179162583769 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/256_stddev",
            "value": 1.943274646949564,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1.938269314025962 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/256_cv",
            "value": 0.0029463430693082667,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0029388388448503768 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/4096_mean",
            "value": 10266.513300853632,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 10266.092794607624 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/4096_median",
            "value": 10263.08022271892,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 10262.770211363179 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/4096_stddev",
            "value": 21.577639785617926,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 21.407670132740378 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/4096_cv",
            "value": 0.0021017495573520378,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0020852792353469652 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/256_mean",
            "value": 215.89961590842518,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 215.8881630693812 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/256_median",
            "value": 217.4740192537375,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 217.4601415396425 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/256_stddev",
            "value": 5.58300964565581,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5.584333754496605 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/256_cv",
            "value": 0.025859284752150137,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.025866789892977762 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/4096_mean",
            "value": 30776.794968228678,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 30775.572368194942 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/4096_median",
            "value": 35033.437489264164,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 35031.7410269616 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/4096_stddev",
            "value": 6283.790019575867,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 6283.078662684623 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/4096_cv",
            "value": 0.20417298247145985,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.2041579791762989 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/256_mean",
            "value": 179.61629352521473,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 179.61487479426592 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/256_median",
            "value": 179.54206495617285,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 179.54211372306702 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/256_stddev",
            "value": 0.19165082264955374,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.1888252845263672 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/256_cv",
            "value": 0.0010670013220301173,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0010512786579766908 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/4096_mean",
            "value": 2577.532667958571,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2577.435774164237 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/4096_median",
            "value": 2574.133916684836,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2574.142152766918 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/4096_stddev",
            "value": 8.633094317006258,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8.68537013108525 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/4096_cv",
            "value": 0.0033493636857932618,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.003369771700286724 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/256_mean",
            "value": 1403.8955011746398,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1403.8600701180385 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/256_median",
            "value": 1405.2004797554496,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1405.2013574773368 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/256_stddev",
            "value": 6.706595411835117,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 6.656527758253382 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/256_cv",
            "value": 0.004777132917815968,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.004741589208170649 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/4096_mean",
            "value": 33109.531807996806,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 33107.567331052785 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/4096_median",
            "value": 33010.40842079841,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 33008.987970280075 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/4096_stddev",
            "value": 191.54605244188843,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 190.55953595944382 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/4096_cv",
            "value": 0.005785223830788969,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.005755769792868808 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/256_mean",
            "value": 1854.105661500722,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1854.0781470971615 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/256_median",
            "value": 1852.8002235762772,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1852.7448785876543 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/256_stddev",
            "value": 5.34605766685117,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5.361270464020089 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/256_cv",
            "value": 0.002883361923680253,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0028916097589597105 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/4096_mean",
            "value": 35840.16459412626,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 35838.243177892946 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/4096_median",
            "value": 40158.193436954374,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 40154.04620034459 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/4096_stddev",
            "value": 5918.801702232915,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5919.015307596363 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/4096_cv",
            "value": 0.1651443783604423,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.16515919260371406 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/256_mean",
            "value": 2029.2168947656435,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2029.1402650386303 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/256_median",
            "value": 2028.433508868044,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2028.4300873818079 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/256_stddev",
            "value": 3.873143183104034,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3.919614434107151 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/256_cv",
            "value": 0.0019086886143589634,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0019316626364577762 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/4096_mean",
            "value": 32413.259054214304,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 32412.31077277938 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/4096_median",
            "value": 32417.729296422778,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 32417.715109574085 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/4096_stddev",
            "value": 13.057919592206057,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.347659971927067 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/4096_cv",
            "value": 0.00040285734829581397,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0004118083423764017 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/256_mean",
            "value": 2077.2145185951495,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2077.1469867415326 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/256_median",
            "value": 2078.091509399105,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2078.0284891236283 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/256_stddev",
            "value": 5.014775223254948,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5.100056499927067 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/256_cv",
            "value": 0.0024141826365851296,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0024553180552367364 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/4096_mean",
            "value": 33100.15489872954,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 33098.7443476217 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/4096_median",
            "value": 33026.88754121327,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 33026.16733396171 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/4096_stddev",
            "value": 263.28459873495575,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 263.2352840269331 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/4096_cv",
            "value": 0.00795418026110389,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.007953029313205583 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/256_mean",
            "value": 463.7315937437826,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 463.712506434375 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/256_median",
            "value": 461.40020952946406,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 461.4008975120576 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/256_stddev",
            "value": 5.8809659110842905,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5.8925316370594985 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/256_cv",
            "value": 0.012681831452557009,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.012707295048755417 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/4096_mean",
            "value": 7131.548478742498,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 7131.198755451257 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/4096_median",
            "value": 7145.684272270349,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 7145.177144168893 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/4096_stddev",
            "value": 55.77142650747088,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 55.80750182671177 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/4096_cv",
            "value": 0.00782038103978577,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.007825823363014696 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/256_mean",
            "value": 457.4518018487544,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 457.42775146734056 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/256_median",
            "value": 455.9826199114984,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 455.9336928127922 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/256_stddev",
            "value": 6.408368275159816,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 6.4103358789515 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/256_cv",
            "value": 0.014008838197294043,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.01401387619878412 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/4096_mean",
            "value": 7107.5894537645,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 7107.22076002455 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/4096_median",
            "value": 7147.5725245499925,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 7146.7829889525165 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/4096_stddev",
            "value": 59.57394222216436,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 59.40827801255769 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/4096_cv",
            "value": 0.008381736538062325,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.008358862066970953 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/256_mean",
            "value": 722.6520685110975,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 722.6261410788431 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/256_median",
            "value": 723.2587826778251,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 723.2574853536328 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/256_stddev",
            "value": 1.8143039278828634,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1.8158211125185701 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/256_cv",
            "value": 0.002510618881394652,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.002512808504004081 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/4096_mean",
            "value": 11353.614302654109,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 11353.105066034319 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/4096_median",
            "value": 11380.741341947505,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 11380.771108476072 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/4096_stddev",
            "value": 69.16098385684063,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 69.56848922621556 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/4096_cv",
            "value": 0.006091538959595714,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0061277059290455495 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_Scalar/4096_mean",
            "value": 1773.2131169250065,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1773.176558659086 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_Scalar/4096_median",
            "value": 1755.8833767061958,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1755.8914985807473 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_Scalar/4096_stddev",
            "value": 45.63177412828356,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 45.62333861570049 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_Scalar/4096_cv",
            "value": 0.025733947991212293,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.025729721269382128 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_LL/4096_mean",
            "value": 2128.332289518662,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2128.2383555247507 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_LL/4096_median",
            "value": 2155.086192540264,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2154.90275713406 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_LL/4096_stddev",
            "value": 58.70511463212498,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 58.69181269999086 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_LL/4096_cv",
            "value": 0.027582682892717647,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0275776501009068 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_Scalar/4096_mean",
            "value": 1840.767413166977,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1840.7371025475964 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_Scalar/4096_median",
            "value": 1834.9645521655561,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1834.9085843155833 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_Scalar/4096_stddev",
            "value": 12.139761429850514,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.156987807131255 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_Scalar/4096_cv",
            "value": 0.006594945859544782,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.006604412868250376 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_LL/4096_mean",
            "value": 2117.93633590822,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2117.858564314945 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_LL/4096_median",
            "value": 2075.728573442603,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2075.7303545058103 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_LL/4096_stddev",
            "value": 60.78178856636755,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 60.810483845917574 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_LL/4096_cv",
            "value": 0.02869859095188662,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.028713194011417704 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_Scalar/4096_mean",
            "value": 2506.919393607658,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2506.782535340596 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_Scalar/4096_median",
            "value": 2471.9994087850196,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2471.789342251791 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_Scalar/4096_stddev",
            "value": 49.737594422722864,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 49.63662165587415 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_Scalar/4096_cv",
            "value": 0.01984012511513044,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.019800928463517494 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_LL/4096_mean",
            "value": 2920.506783547903,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2920.4279859821977 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_LL/4096_median",
            "value": 2952.9432139698365,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2952.826117602578 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_LL/4096_stddev",
            "value": 72.29447390212806,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 72.30316508721128 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_LL/4096_cv",
            "value": 0.0247540852530748,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.024757729152802336 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_Scalar/4096_mean",
            "value": 8799.680437936977,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8799.367943459101 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_Scalar/4096_median",
            "value": 8831.936581054686,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8831.956521739115 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_Scalar/4096_stddev",
            "value": 47.64754649871957,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 47.74753632727639 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_Scalar/4096_cv",
            "value": 0.005414690548682039,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.005426246138822836 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_LL/4096_mean",
            "value": 8744.817469841784,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8744.560622538873 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_LL/4096_median",
            "value": 8744.442308895204,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8744.22623289012 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_LL/4096_stddev",
            "value": 8.50988953949184,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8.410739665223634 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_LL/4096_cv",
            "value": 0.0009731351819337409,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0009618253024108696 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_Scalar/4096_mean",
            "value": 8762.274002969263,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8762.146717609987 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_Scalar/4096_median",
            "value": 8749.574157696614,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8749.228772616949 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_Scalar/4096_stddev",
            "value": 40.78649357450747,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 40.88121519212552 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_Scalar/4096_cv",
            "value": 0.004654784084666399,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.004665662024348814 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_LL/4096_mean",
            "value": 8784.277689467352,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8783.986907816565 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_LL/4096_median",
            "value": 8743.506414671303,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8743.505694974532 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_LL/4096_stddev",
            "value": 59.30703596224991,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 59.02757346946811 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_LL/4096_cv",
            "value": 0.006751498308547449,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.006719906813265116 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax_Scalar/4096_mean",
            "value": 3279.8916432966735,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3279.7992521552687 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax_Scalar/4096_median",
            "value": 3254.3974771778676,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3254.216251774527 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax_Scalar/4096_stddev",
            "value": 54.137039544255934,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 54.21750946703448 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax_Scalar/4096_cv",
            "value": 0.016505740259712333,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.016530740237045392 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin_Scalar/4096_mean",
            "value": 3584.767251127652,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3584.6849968155825 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin_Scalar/4096_median",
            "value": 3551.019912395118,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3550.7573469204167 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin_Scalar/4096_stddev",
            "value": 52.051509930235476,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 52.04914965217883 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin_Scalar/4096_cv",
            "value": 0.014520192325976462,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.01451986707295513 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_Scalar/4096_mean",
            "value": 1786.3842326916554,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1786.302496875768 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_Scalar/4096_median",
            "value": 1764.1924706323448,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1764.054386403365 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_Scalar/4096_stddev",
            "value": 38.3552419412802,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 38.36075152870084 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_Scalar/4096_cv",
            "value": 0.0214708802503748,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.021474947046087406 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_LL/4096_mean",
            "value": 2088.075952833665,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2088.0184316504337 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_LL/4096_median",
            "value": 2068.7150037047254,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2068.6973001706565 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_LL/4096_stddev",
            "value": 43.96818949317411,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 44.00128003987319 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_LL/4096_cv",
            "value": 0.021056796058355157,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.021073223958609038 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_Scalar/4096_mean",
            "value": 1853.2048758543096,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1853.0807489972108 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_Scalar/4096_median",
            "value": 1853.8683944166194,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1853.8342996738597 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_Scalar/4096_stddev",
            "value": 6.288862700379801,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 6.263220032323055 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_Scalar/4096_cv",
            "value": 0.003393506450537853,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0033798959034636662 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_LL/4096_mean",
            "value": 2070.1923571317893,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2070.0754931540523 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_LL/4096_median",
            "value": 2069.785362165717,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2069.4568596781587 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_LL/4096_stddev",
            "value": 4.44510171744816,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 4.3653849848553685 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_LL/4096_cv",
            "value": 0.0021471926036896205,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.002108804726828628 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_Scalar/4096_mean",
            "value": 1750.3525374364676,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1750.2541823954566 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_Scalar/4096_median",
            "value": 1751.8010549735095,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1751.8052736180155 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_Scalar/4096_stddev",
            "value": 5.990552938282437,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 6.0309561771028255 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_Scalar/4096_cv",
            "value": 0.0034224836483832477,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0034457601860140435 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_LL/4096_mean",
            "value": 2101.883268661032,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2101.8158827014713 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_LL/4096_median",
            "value": 2072.765521327043,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2072.763721860306 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_LL/4096_stddev",
            "value": 50.37311520531212,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 50.37321187635213 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_LL/4096_cv",
            "value": 0.02396570540161416,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.023966519756053635 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenEqualBitmap/4096_mean",
            "value": 1859.3941279206906,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1859.3406705655307 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenEqualBitmap/4096_median",
            "value": 1860.5110746514933,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1860.466180634527 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenEqualBitmap/4096_stddev",
            "value": 4.519670380002067,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 4.503056371636042 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenEqualBitmap/4096_cv",
            "value": 0.002430722089596083,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0024218565445924484 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenNotEqualBitmap/4096_mean",
            "value": 1828.4325632127966,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1828.3472217841384 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenNotEqualBitmap/4096_median",
            "value": 1826.9939086894003,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1826.9635375598634 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenNotEqualBitmap/4096_stddev",
            "value": 4.231751154403385,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 4.228706490113787 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenNotEqualBitmap/4096_cv",
            "value": 0.0023144146738273146,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.002312857448372049 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeBitmap/4096_mean",
            "value": 1861.45731917725,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1861.421082946234 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeBitmap/4096_median",
            "value": 1854.7667020568872,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1854.7704114134133 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeBitmap/4096_stddev",
            "value": 16.676255962282806,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 16.59455627557678 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeBitmap/4096_cv",
            "value": 0.008958709818634782,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.008914993188597137 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeEqualBitmap/4096_mean",
            "value": 1856.650474576781,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1856.5798586197973 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeEqualBitmap/4096_median",
            "value": 1856.4721475757037,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1856.3896427106242 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeEqualBitmap/4096_stddev",
            "value": 6.010896164612229,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 6.016995852103575 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeEqualBitmap/4096_cv",
            "value": 0.003237494750315026,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0032409033331734405 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessBitmap/4096_mean",
            "value": 1854.9628056494141,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1854.8970583145588 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessBitmap/4096_median",
            "value": 1855.5301822247777,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1855.368742263067 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessBitmap/4096_stddev",
            "value": 1.64809185836361,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1.6504629226456196 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessBitmap/4096_cv",
            "value": 0.0008884770375687508,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0008897868025869335 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessEqualBitmap/4096_mean",
            "value": 1857.4326077166006,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1857.3834039959966 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessEqualBitmap/4096_median",
            "value": 1854.7928149348463,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1854.7024484604833 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessEqualBitmap/4096_stddev",
            "value": 7.423386288232152,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 7.4436148932818655 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessEqualBitmap/4096_cv",
            "value": 0.003996584456088531,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.004007581244274922 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfEqual/4096_mean",
            "value": 2531.8375866316123,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2531.7017160013415 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfEqual/4096_median",
            "value": 2509.966039533741,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2509.553686841041 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfEqual/4096_stddev",
            "value": 53.754470039078385,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 53.836994393658266 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfEqual/4096_cv",
            "value": 0.021231405332991355,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.021265141171010582 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfNotEqual/4096_mean",
            "value": 2471.2965184936875,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2471.239538672425 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfNotEqual/4096_median",
            "value": 2440.702162688659,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2440.5923474094375 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfNotEqual/4096_stddev",
            "value": 76.22146826470832,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 76.23369651823819 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfNotEqual/4096_cv",
            "value": 0.03084270450523156,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.03084836387782615 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLarge/4096_mean",
            "value": 2498.5157482358163,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2495.8607461601828 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLarge/4096_median",
            "value": 2514.390999030993,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2514.2285699460053 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLarge/4096_stddev",
            "value": 53.15301263718995,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 50.779266776975305 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLarge/4096_cv",
            "value": 0.02127383534593325,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.020345392608581186 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLargeEqual/4096_mean",
            "value": 2404.9954896258205,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2404.8933645631478 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLargeEqual/4096_median",
            "value": 2407.0398718506185,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2406.8017535358013 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLargeEqual/4096_stddev",
            "value": 4.010139104091811,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3.9813327766266395 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLargeEqual/4096_cv",
            "value": 0.001667420634005316,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0016555132278598369 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLess/4096_mean",
            "value": 2421.757560436511,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2421.6694111900165 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLess/4096_median",
            "value": 2399.697608254362,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2399.700247658464 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLess/4096_stddev",
            "value": 49.01053721595172,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 48.910278260843874 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLess/4096_cv",
            "value": 0.02023759025949641,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.020196926151373074 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLessEqual/4096_mean",
            "value": 2442.757218509195,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2442.6087969151877 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLessEqual/4096_median",
            "value": 2416.4493144818657,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2416.1248500429256 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLessEqual/4096_stddev",
            "value": 43.986314397816834,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 44.03639394062226 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLessEqual/4096_cv",
            "value": 0.018006830177196863,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.01802842681817759 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfByBitmap/4096_mean",
            "value": 4156.97901089582,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 4156.866531214213 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfByBitmap/4096_median",
            "value": 4191.948347479767,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 4191.952697007816 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfByBitmap/4096_stddev",
            "value": 57.60715638250789,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 57.5547151107751 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfByBitmap/4096_cv",
            "value": 0.013857937755161693,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.01384569715640196 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FilterByBitmap/4096_mean",
            "value": 1838.5665761788848,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1838.4639312381137 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FilterByBitmap/4096_median",
            "value": 1852.4400955757526,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1852.3086848306223 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FilterByBitmap/4096_stddev",
            "value": 29.513908728206058,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 29.417336563673935 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FilterByBitmap/4096_cv",
            "value": 0.016052673376421956,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.01600104090367594 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaReuse_mean",
            "value": 13.681428635870706,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.680809199573869 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaReuse_median",
            "value": 13.679307825801846,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.679314570388351 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaReuse_stddev",
            "value": 0.006607460302404999,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0065224804024409085 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaReuse_cv",
            "value": 0.00048295104833432414,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00047676130171043325 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaFresh_mean",
            "value": 14.008191565092208,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.007801786965132 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaFresh_median",
            "value": 14.011219105395005,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.011217404179186 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaFresh_stddev",
            "value": 0.011597338576117736,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.011620301928543601 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaFresh_cv",
            "value": 0.0008278969146180005,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0008295592774133053 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaReuse_mean",
            "value": 33.64586533732327,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 33.643936188453786 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaReuse_median",
            "value": 33.65267640757255,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 33.650660862779986 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaReuse_stddev",
            "value": 0.02275671256572074,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.022542267091470173 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaReuse_cv",
            "value": 0.0006763598539543811,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0006700246655207492 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaFresh_mean",
            "value": 33.43819826529416,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 33.4368224909043 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaFresh_median",
            "value": 33.3452137819514,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 33.343995853902754 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaFresh_stddev",
            "value": 0.15025157004295267,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.14962881123915264 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaFresh_cv",
            "value": 0.004493411063923868,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0044749710077820835 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/2_mean",
            "value": 35.784428518982914,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 35.78295907964336 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/2_median",
            "value": 35.7830072940104,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 35.77982274185195 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/2_stddev",
            "value": 0.021047542729935222,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.020880317276394215 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/2_cv",
            "value": 0.0005881760195994167,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0005835268466735849 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/8_mean",
            "value": 106.12850768290666,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 106.12458140575453 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/8_median",
            "value": 106.11729065923593,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 106.11334547799979 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/8_stddev",
            "value": 0.05685762202539786,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.057153564389821256 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/8_cv",
            "value": 0.0005357431595597145,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0005385516120087343 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/32_mean",
            "value": 573.5316065617607,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 573.4974576322589 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/32_median",
            "value": 579.3001127915987,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 579.2747680886575 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/32_stddev",
            "value": 17.487490396955252,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 17.5041144704118 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/32_cv",
            "value": 0.03049089221392738,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.030521694974340895 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/2_mean",
            "value": 25.485297262489297,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 25.48521298737007 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/2_median",
            "value": 25.479876012626264,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 25.47993149753842 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/2_stddev",
            "value": 0.013936242526902922,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.013727831000995908 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/2_cv",
            "value": 0.0005468346075529232,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00053865867268989 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/8_mean",
            "value": 95.56440290436197,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 95.56373466748889 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/8_median",
            "value": 95.52376788501599,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 95.52175519730905 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/8_stddev",
            "value": 0.17115270191540777,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.1704985760406214 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/8_cv",
            "value": 0.0017909671040030703,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.001784134710032692 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/32_mean",
            "value": 543.8600739730198,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 543.8467887377558 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/32_median",
            "value": 543.6787222551694,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 543.6788135560365 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/32_stddev",
            "value": 0.5899101302125367,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.5875337321369035 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/32_cv",
            "value": 0.0010846726179090714,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0010803295051176878 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_i64_mean",
            "value": 0.35180603904167634,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.35179472229428155 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_i64_median",
            "value": 0.35175764948694843,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.35175878473328037 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_i64_stddev",
            "value": 0.00031160194180462094,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0003111934369158559 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_i64_cv",
            "value": 0.0008857208439440897,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0008845881339161704 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_f64_mean",
            "value": 0.31138340684706245,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.31138428459190476 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_f64_median",
            "value": 0.3113856760031316,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.31138708630724204 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_f64_stddev",
            "value": 0.0003350392312565243,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00033527063168620425 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_f64_cv",
            "value": 0.0010759700866818525,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0010767101882665803 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/16_mean",
            "value": 2.18583263806831,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2.1857953000979404 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/16_median",
            "value": 2.1808414805223437,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2.180778594208437 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/16_stddev",
            "value": 0.01114105384738498,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.011164408443701816 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/16_cv",
            "value": 0.00509693818884079,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.005107709968633186 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/256_mean",
            "value": 20.403231166661676,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 20.40309189577394 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/256_median",
            "value": 20.400474300769186,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 20.400544581121448 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/256_stddev",
            "value": 0.017541696569694818,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.017260392497994458 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/256_cv",
            "value": 0.0008597509103537223,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0008459694533635647 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/4096_mean",
            "value": 329.07416243959716,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 329.0684959423482 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/4096_median",
            "value": 328.8984820080342,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 328.8806126437606 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/4096_stddev",
            "value": 0.36064275989730443,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.3579006648928043 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/4096_cv",
            "value": 0.0010959315590858697,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0010876175304107732 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}