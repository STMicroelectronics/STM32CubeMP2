---
pagetitle: Package License Terms for STM32CubeMP2
lang: en
header-includes: <link rel="icon" type="image/x-icon" href="_htmresc/favicon.png" />
---

::: {.row}
::: {.col-sm-12 .col-lg-3}

<center>
# These are full license terms for

# <mark>STM32CubeMP2</mark>

Copyright &copy; 2024 STMicroelectronics

All rights reserved
    
[![ST logo](_htmresc/st_logo_2020.png)](https://www.st.com){.logo}
</center>
:::

::: {.col-sm-12 .col-lg-9}


## <mark>__OVERVIEW__</mark>

<div>

This Software Bill Of Material (SBOM) lists the software components of this
software package, including the copyright owner and license terms for each
component.

The full text of these licenses are below the SBOM.

__SOFTWARE BILL OF MATERIALS__


| Component                   | Copyright                                   | License
| --------------------        | --------------------------                  | ----------------------
| CMSIS Core                  | ARM Limited                                 | [Apache License 2.0](https://opensource.org/licenses/Apache-2.0)
| CMSIS Device                | ARM Limited, STMicroelectronics             | [Apache License 2.0](https://opensource.org/licenses/Apache-2.0)
| Drivers HAL/LL              | STMicroelectronics                          | [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)
| Drivers BSP Boards          | STMicroelectronics                          | [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)
| BSP Components              | STMicroelectronics                          | [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)
| STM32 USBPD Core Library    | STMicroelectronics                          | [SLA](#collapse-section1)
| STM32 USBPD Device Library  | STMicroelectronics                          | [SLA](#collapse-section1)
| OpenAMP                     | Xilinx Inc. and Contributors                | [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)
| FreeRTOS Kernel             | MIT                                         | [MIT](https://opensource.org/licenses/MIT)
| STM32 Projects              | STMicroelectronics                          | [SLA](#collapse-section1) (BSD-3-Clause for basic Examples)
| STM32 Utilities             | STMicroelectronics                          | [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)
| LwIP                        | Swedish Institute of Computer Science, STMicroelectronics  | [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)
| MbedTLS                     | Arm Limited (or its affiliates)             | [Apache-2.0](https://opensource.org/licenses/Apache-2.0)
| Utilities lcd               | STMicroelectronics                          | [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)
| Utilities Fonts             | STMicroelectronics                          | [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)
| Utilities CoproSync         | STMicroelectronics                          | [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)
| Utilities GUI_INTERFACE     | STMicroelectronics                          | [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)
| Utilities ImageHeader       | STMicroelectronics                          | [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)
| Utilities M33TD_NSAppCore   | STMicroelectronics                          | [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)
| Utilities ResourcesManager  | STMicroelectronics                          | [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)
| Utilities TRACER_EMB        | STMicroelectronics                          | [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)
| scmiagent                   | ARM Limited                                 | [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)


__Notes:__ If the license is an open source license, then you can access the
terms at [www.opensource.org](https://opensource.org/). Otherwise, the full
license terms are below. If a component is not listed in the SBOM, then the SLA
shall apply unless other terms are clearly stated in the package.


</div>



::: {.collapse}
<input type="checkbox" id="collapse-section1" checked aria-hidden="true">
<label for="collapse-section1" aria-hidden="true">__SLA – Software License Agreement__</label>
<div>

SLA0048 Rev5/October 2025

## Software license agreement

### __SOFTWARE PACKAGE LICENSE AGREEMENT (“AGREEMENT”)__

BY CLICKING ON THE "I ACCEPT" BUTTON OR BY  UNZIPPING, INSTALLING, COPYING, DOWNLOADING, ACCESSING OR OTHERWISE USING THIS SOFTWARE PACKAGE OR ANY PART THEREOF, INCLUDING ANY RELATED DOCUMENTATION (collectively the “SOFTWARE PACKAGE”) FROM STMICROELECTRONICS INTERNATIONAL N.V, SWISS BRANCH AND/OR ITS AFFILIATED COMPANIES (collectively "STMICROELECTRONICS”), YOU (hereinafter referred also to as “THE RECIPIENT”), ON BEHALF OF YOURSELF, OR ON BEHALF OF ANY ENTITY BY WHICH YOU ARE EMPLOYED AND/OR ENGAGED, AGREE TO BE BOUND BY THIS AGREEMENT.

Under STMICROELECTRONICS’ intellectual property rights and subject to  applicable licensing terms for any third-party software incorporated in the SOFTWARE PACKAGE and applicable Open Source Terms (as defined here below), the redistribution, reproduction and use in source and binary forms of the SOFTWARE PACKAGE or any part thereof, with or without modification, are permitted provided that the following conditions are met:

1. Redistribution of source code (modified or not) must retain any copyright notice accompanying the SOFTWARE PACKAGE, this list of conditions and the disclaimer below.

2. Redistributions in binary form, except as embedded into a processing unit device manufactured by or for STMicroelectronics or a software update for any such device, must reproduce the accompanying copyright notice, this list of conditions and the below disclaimer in capital type, in the documentation and/or other materials provided with the distribution.

3. Neither the name of STMicroelectronics nor the names of other contributors to the SOFTWARE PACKAGE may be used to endorse or promote products derived from the SOFTWARE PACKAGE or part thereof without specific written permission.

4. The SOFTWARE PACKAGE or any part thereof, including modifications and/or derivative works of the SOFTWARE PACKAGE, must be used and execute solely and exclusively on or in combination with a processing unit device manufactured by or for STMicroelectronics.

5. No use, reproduction or redistribution of the SOFTWARE PACKAGE partially or totally may be done in any manner that would subject the SOFTWARE PACKAGE to any Open Source Terms (as defined below).

6. Some portion of the SOFTWARE PACKAGE may contain software subject to Open Source Terms (as defined below) applicable for each such portion (“Open Source Software”), as further specified in the SOFTWARE PACKAGE. Such Open Source Software is supplied under the applicable Open Source Terms and is not subject to the terms and conditions of this AGREEMENT. “Open Source Terms” shall mean any open source license which requires as part of distribution of software that the source code of such software is distributed therewith or otherwise made available, or open source license that substantially complies with the Open Source definition specified at www.opensource.org and any other comparable open source license such as for example GNU General Public License (GPL), Eclipse Public License (EPL), Apache Software License, BSD license and MIT license.

7. The SOFTWARE PACKAGE may also include third party software as expressly specified in the SOFTWARE PACKAGE subject to specific license terms from such third parties. Such third-party software is supplied under such specific license terms and is not subject to the terms and conditions of this AGREEMENT. By installing copying, downloading, accessing or otherwise using the SOFTWARE PACKAGE, the RECIPIENT agrees to be bound by such license terms with regard to such third-party software.

8. STMicroelectronics has no obligation to provide any maintenance, support or updates for the SOFTWARE PACKAGE.

9. The SOFTWARE PACKAGE is and will remain the exclusive property of STMicroelectronics and its licensors. The RECIPIENT will not take any action that jeopardizes STMicroelectronics and its licensors' proprietary rights or acquire any rights in the SOFTWARE PACKAGE, except the limited rights specified hereunder.

10. The RECIPIENT shall comply with all applicable laws and regulations affecting the use of the SOFTWARE PACKAGE or any part thereof including any applicable export control law or regulation.

11. Redistribution and use of the SOFTWARE PACKAGE partially or any part thereof other than as permitted under this AGREEMENT is void and will automatically terminate RECIPIENT’s rights under this AGREEMENT.

12. The RECIPIENT shall be solely liable to determine and verify that the SOFTWARE PACKAGE is fit for the RECIPIENT intended use, environment or application and comply with all regulatory, safety and security related requirements concerning any use.  

DISCLAIMER:

THE SOFTWARE PACKAGE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THE SOFTWARE PACKAGE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EXCEPT AS EXPRESSLY PERMITTED HEREUNDER AND SUBJECT TO THE APPLICABLE LICENSING TERMS FOR ANY THIRD-PARTY SOFTWARE INCORPORATED IN THE SOFTWARE PACKAGE AND OPEN SOURCE TERMS AS APPLICABLE, NO LICENSE OR OTHER RIGHTS, WHETHER EXPRESS OR IMPLIED, ARE GRANTED UNDER ANY PATENT OR OTHER INTELLECTUAL PROPERTY RIGHTS OF STMICROELECTRONICS OR ANY THIRD PARTY.

</div>
:::



:::
:::
