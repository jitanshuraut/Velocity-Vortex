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
} from 'echarts/components';

echarts.use([
  LineChart,
  TitleComponent,
  TooltipComponent,
  GridComponent,
  LegendComponent,
  CanvasRenderer,
]);

interface ActivityChartProps {
  data: number[];
  sx?: SxProps;
}

const ActivityChart = ({ data, ...rest }: ActivityChartProps) => {
  const theme = useTheme();

  const option = useMemo(
    () => ({
      grid: {
        top: 10,
        bottom: 10,
        left: 5,
        right: 5,
        containerLabel: true,
      },
      xAxis: {
        type: 'category',
        data: ['', '', '', '', ''],
        axisTick: {
          show: false,
        },
        axisLine: {
          show: false,
        },
        boundaryGap: 0,
      },
      yAxis: {
        type: 'value',
        min: 10,
        minInterval: 1,
        axisLabel: {
          show: false,
        },
        splitLine: {
          show: false,
        },
      },
      series: [
        {
          data,
          type: 'line',
          smooth: true,
          showSymbol: false,
          lineStyle: {
            color: new echarts.graphic.LinearGradient(0, 0, 1, 0, [
              { offset: 1, color: 'rgba(84, 112, 198, 0.2)' },
              { offset: 0, color: theme.palette.info.light },
            ]),
            width: 3,
            type: 'solid',
            cap: 'round',
          },
        },
      ],
    }),
    [theme, data],
  );

  return <ReactEchart echarts={echarts} option={option} {...rest} />;
};

export default ActivityChart;
