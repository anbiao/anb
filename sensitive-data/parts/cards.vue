<template>
  <div id="cards">
    <div
      class="card-item"
      v-for="(card, i) in cards"
      :key="card.title"
    >
      <div class="card-header">
        <div class="card-header-right">{{ '0' + (i + 1) }}</div>
      </div>
      <dv-charts class="ring-charts" :option="card.ring" />
      <div class="card-footer">
        <div class="card-footer-item">
          <div class="footer-title">风险1</div>
          <div class="footer-detail">
            <dl>{{card.total.name}}</dl>
          </div>
        </div>
        <div class="card-footer-item">
          <div class="footer-title">风险2</div>
          <div class="footer-detail">
            <dl>{{card.num.name}}</dl>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'Cards',
  data () {
    return {
      cards: []
    }
  },
  methods: {
    createData () {
      const { randomExtend } = this
      var  strAry=["价格波动风险","流动性风险","投机性风险","套期保值风险","交易履约风险","现金流风险","安全管理风险","技术风险","信息安全风险","信用风险","合同纠纷","仓库质押","业务运营","贷款不足","政策风险"]
      this.cards = new Array(5).fill(0).map((foo, i) => ({
        total: {

         name: strAry[i],
        },
        num: {
          name: strAry[i+5],
          content: '{nt}',
          style: {
            fill: '#26fcd8',
            fontWeight: 'bold'
          }
        },
        ring: {
          series: [
            {
              type: 'gauge',
              startAngle: -Math.PI / 2,
              endAngle: Math.PI * 1.5,
              arcLineWidth: 13,
              radius: '80%',
              data: [
                { name:'相关系数', value: randomExtend(40, 60) }
              ],
              axisLabel: {
                show: false
              },
              axisTick: {
                show: false
              },
              pointer: {
                show: false
              },
              backgroundArc: {
                style: {
                  stroke: '#224590'
                }
              },
              details: {
                show: true,
                formatter: '{name}{value}%',
                style: {
                  fill: '#1ed3e5',
                  fontSize: 20
                }
              }
            }
          ],
          color: ['#03d3ec']
        }
      }))
    },
    randomExtend (minNum, maxNum) {
      if (arguments.length === 1) {
        return parseInt(Math.random() * minNum + 1, 10)
      } else {
        return parseInt(Math.random() * (maxNum - minNum + 1) + minNum, 10)
      }
    }
  },
  mounted () {
    const { createData } = this

    createData()

    setInterval(this.createData, 30000)
  }
}
</script>

<style lang="less">
#cards {
  display: flex;
  justify-content: space-between;
  height: 45%;

  .card-item {
    background-color: rgba(6, 30, 93, 0.5);
    border-top: 2px solid rgba(1, 153, 209, .5);
    width: 19%;
    display: flex;
    flex-direction: column;
  }

  .card-header {
    display: flex;
    height: 20%;
    align-items: center;
    justify-content: space-between;

    .card-header-right {
      padding-right: 20px;
      font-size: 40px;
      color: #03d3ec;
      margin-left: 15px;
    }
  }

  .ring-charts {
    height: 50%;
  }

  .card-footer {
    height: 30%;
    display: flex;
    align-items: center;
    justify-content: space-around;
  }

  .card-footer-item {
    box-sizing: border-box;
    width: 40%;
    background-color: rgba(6, 30, 93, 0.7);

    .footer-title {
      display: flex;
      font-size: 15px;
      margin: 5px;
    }

    .footer-detail {
      font-size: 15px;
      color: #1294fb;
    }
  }
}
</style>
