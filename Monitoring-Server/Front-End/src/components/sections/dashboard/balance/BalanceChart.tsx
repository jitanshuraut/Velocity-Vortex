import { useMemo } from 'react';
import { SxProps, useTheme } from '@mui/material';
import * as echarts from 'echarts/core';
import ReactEchart from 'components/base/ReactEchart';
import { CanvasRenderer } from 'echarts/renderers';
import { LineChart } from 'echarts/charts';

import {
  TitleComponent,
  TooltipComponent,
  GridComponent,
  LegendComponent,
  DataZoomComponent,
  ToolboxComponent,
} from 'echarts/components';

echarts.use([
  LineChart,
  TitleComponent,
  TooltipComponent,
  GridComponent,
  LegendComponent,
  CanvasRenderer,
  DataZoomComponent,
  ToolboxComponent,
]);

interface ClientChartProps {
  date: string[];
  chartData: number[];
  sx?: SxProps;
}

const BalanceChart = ({ date, chartData, ...rest }: ClientChartProps) => {
  const theme = useTheme();

  const option = useMemo(
    () => ({
      tooltip: {
        trigger: 'axis',
        position: function (pt: number[]) {
          return [pt[0], '10%'];
        },
      },
      title: {
        left: 'center',
      },
      toolbox: {
        feature: {
          dataZoom: {
            yAxisIndex: 'none',
          },
          restore: {},
          saveAsImage: {},
        },
      },
      xAxis: {
        type: 'category',
        boundaryGap: false,
        data: date,
      },
      yAxis: {
        type: 'value',
        boundaryGap: [0, '100%'],
      },
      dataZoom: [
        {
          type: 'inside',
          start: 0,
          end: 10,
        },
        {
          start: 0,
          end: 10,
        },
        {
          type: 'inside',
          yAxisIndex: 0,
          start: 0,
          end: 100,
        },
        {
          yAxisIndex: 0,
          start: 0,
          end: 100,
        },
      ],
      series: [
        {
          name: 'Fake Data',
          type: 'line',
          symbol: 'none',
          sampling: 'lttb',
          itemStyle: {
            color: 'rgb(134, 140, 255)',
          },
          areaStyle: {
            color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [
              {
                offset: 0,
                color: '#868CFF',
              },
              {
                offset: 1,
                color: '#4318FF',
              },
            ]),
          },
          data: chartData,
        },
      ],
    }),
    [theme, date, chartData],
  );

  return <ReactEchart echarts={echarts} option={option} {...rest} />;
};

export default BalanceChart;
