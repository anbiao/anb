<template>
  <div id="rose-chart">
    <div class="region-title">
      <canvas id="graph-title" ref="graph-title" style="width: 100%; height: 24px;" ></canvas>
    </div>
    <dv-charts :option="option" />
  </div>
</template>

<script>
  import CRender from '@jiaminghi/c-render'
export default {
  name: 'RoseChart',
  components: {
    CRender,
  },
  data () {
    return {
      option: {}
    }
  },
  methods: {
    createData () {
      const { randomExtend } = this
      this.option = {
        series: [
          {
            type: 'pie',
            radius: '50%',
            roseSort: false,
            data: [
              { name: '套期保值风险', value: randomExtend(40, 70) },
              { name: '交易履约风险', value: randomExtend(20, 30) },
              { name: '流动性风险', value: randomExtend(10, 50) },
              { name: '价格波动风险', value: randomExtend(5, 20) },
              { name: '投机性风险', value: randomExtend(40, 50) },
              { name: '业务运营风险', value: randomExtend(20, 30) },
              { name: '法律性风险', value: randomExtend(5, 10) }
            ],
            insideLabel: {
              show: false
            },
            outsideLabel: {
              formatter: '{name} {percent}%',
              labelLineEndLength: 20,
              style: {
                fontSize: 18,
                color: ['#0000FF', '#00FFFF', '#FFFF00', '#FFE4C4', '#BA55D3', '#7FFF00',  '#b72700']

              },
              labelLineStyle: {
                color: ['#0000FF', '#00FFFF', '#FFFF00', '#FFE4C4', '#BA55D3', '#7FFF00',  '#b72700']
              }
            },
            roseType: true
          }
        ],
        color: ['#0000FF', '#00FFFF', '#FFFF00', '#FFE4C4', '#BA55D3', '#7FFF00',  '#b72700']
      }
    },
    randomExtend (minNum, maxNum) {
      if (arguments.length === 1) {
        return parseInt(Math.random() * minNum + 1, 10)
      } else {
        return parseInt(Math.random() * (maxNum - minNum + 1) + minNum, 10)
      }
    },init() {
      this.cRender = new CRender(this.$refs['graph-title']);
      this.cRender.delAllGraph();
      this.drawTitle(this.cRender);
    },
    drawTitle(render) {
      const {
        area: [w, h],
      } = render;
      const centerPoint = [w / 2, h / 2];
      const graphTitle = {
        name: 'text',
        animationCurve: 'easeOutBounce',
        hover: true,
        drag: true,
        shape: {
          content: '风险发生可能性系数',
          position: centerPoint,
        },
        style: {
          fill: '#FAFAD2',
          fontSize: 18,
          shadowBlur: 0,
          shadowColor: '#DAA520',
          hoverCursor: 'pointer',
          scale: [1, 1],
          rotate: 0,
        }
      }
      render.add(graphTitle);
    }
  },
  mounted () {
    const { createData } = this
    createData()
    setInterval(createData, 30000)
    this.init()
  }
}
</script>

<style lang="less">
#rose-chart {
  width: 40%;
  height: 100%;
  background-color: rgba(6, 30, 93, 0.5);
  border-top: 2px solid rgba(1, 153, 209, .5);
  box-sizing: border-box;

  .region-title {
    padding-top: 2.4rem;

    #graph-title {
      line-height: 28px;
    }
  }

  .dv-charts-container {
    height: calc(~"100% - 50px");
  }
}
</style>
