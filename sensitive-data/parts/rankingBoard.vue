<template>
  <div id="ranking-board">
    <div class="region-title">
      <canvas id="graph-title" ref="graph-title" style="width: 100%; height: 24px;" ></canvas>
    </div>
    <dv-capsule-chart :config="config" style="width:100%; height:450px;" />
  </div>
</template>

<script>
  import CRender from '@jiaminghi/c-render'
export default {
  name: 'RankingBoard',
  components: {
    CRender,
  },
  data () {
    return {
      config: {
        data: [
          {
            name: '价格波动风险',
            value: 133
          },
          {
            name: '流动性风险',
            value: 123
          },
          {
            name: '投机性风险',
            value: 110
          },
          {
            name: '套期保值风险',
            value: 100
          },
          {
            name: '交易履约风险',
            value: 100
          },
          {
            name: '现金流风险',
            value: 98
          },
          {
            name: '安全管理风险',
            value: 75
          },
          {
            name: '应用安全风险',
            value: 66
          },
          {
            name: '技术风险',
            value: 66
          },
          {
            name: '信息安全风险',
            value: 66
          },
        ],
        unit: '系数',
        showValue: true
      }
    }
  },
  mounted() {
    this.init();
  },
  methods: {
    init() {
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
          content: '交易商风险系数',
          position: centerPoint,
        },
        style: {
          fill: '#9ce5f4',
          fontSize: 18,
          shadowBlur: 0,
          shadowColor: '#66eece',
          hoverCursor: 'pointer',
          scale: [1, 1],
          rotate: 0,
        }
      }
      render.add(graphTitle);
    }
  }
}
</script>

<style lang="less">
#ranking-board {
  width: 25%;
  display: flex;
  flex-direction: column;
  box-shadow: 0 0 3px blue;
  background-color: rgba(6, 30, 93, 0.5);
  border-top: 2px solid rgba(1, 153, 209, .5);
  box-sizing: border-box;
  padding: 0 30px;

  .region-title {
    padding-top: 2.4rem;

    #graph-title {
      line-height: 28px;
    }
  }

  .dv-scroll-ranking-board {
    flex: 1;
  }
}
</style>
